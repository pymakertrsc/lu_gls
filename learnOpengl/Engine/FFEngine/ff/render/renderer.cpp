#include "renderer.h"
#include "../objects/group.h"
#include "../objects/skinnedMesh.h"
#include "../tools/timer.h"
#include "../log/debugLog.h"

namespace ff {

	static void onFrameSizeCallback(DriverWindow* dwindow, int width, int height) {
		if (dwindow->mRenderer != nullptr) {
			dwindow->mRenderer->setSize(width, height);
		}
	}

	Renderer::Renderer(const Descriptor& descriptor) noexcept {
		mWidth = descriptor.mWidth;
		mHeight = descriptor.mHeight;

		mViewport = { 0.0f, 0.0f, mWidth, mHeight };

		mWindow = DriverWindow::create(this, mWidth, mHeight);
		mWindow->setFrameSizeCallBack(onFrameSizeCallback);

		mInfos = DriverInfo::create();
		mRenderList = DriverRenderList::create();
		mAttributes = DriverAttributes::create();
		mState = DriverState::create();
		mBindingStates = DriverBindingStates::create(mAttributes);
		mGeometries = DriverGeometries::create(mAttributes, mInfos, mBindingStates);
		mObjects = DriverObjects::create(mGeometries, mAttributes, mInfos);
		mPrograms = DriverPrograms::create();
		mMaterials = DriverMaterials::create(mPrograms);
		mBackground = DriverBackground::create(this, mObjects);
		mRenderState = DriverRenderState::create();
		mRenderTargets = DriverRenderTargets::create();
		mTextures = DriverTextures::create(mInfos, mRenderTargets);
		mShadowMap = DriverShadowMap::create(this, mObjects, mState);

		mFrustum = Frustum::create();
	}

	Renderer::~Renderer() noexcept {}

	bool Renderer::render(Scene::Ptr scene, Camera::Ptr camera) {
		//��������Ϣ�������ж��Ƿ����render
		if (!mWindow->processEvent()) {
			return false;
		}

		if (scene == nullptr) { scene = mDummyScene; }

		//1 ���³�������
		scene->updateWorldMatrix(true, true);
		camera->updateWorldMatrix(true, true);

		auto projectionMatrix = camera->getProjectionMatrix();
		auto cameraInverseMatrix = camera->getWorldMatrixInverse();

		mCurrentViewMatrix = projectionMatrix * cameraInverseMatrix;
		mFrustum->setFromProjectionMatrix(mCurrentViewMatrix);

		//2 ��ȡ��Ⱦ���ݣ�������Ⱦ�б���״̬
		mRenderState->init();
		mRenderList->init();

		//scene���е����ݶ��ǲ㼶�ܹ�����״���ݣ�������ṹ������Ϊһ�������б�
		projectObject(scene, 0, mSortObject);

		//�������projectObject֮�����п���Ⱦ����&���Ӿ��巶Χ�ڵģ����Ѿ���ѹ�뵽��RenderList����
		mRenderList->finish();

		//��������projectObject�����̣��κ�һ������ʹ�õ���Attribute���Ѿ��ɹ��ı�������Ϊ��һ��VBO
		//�����������У�ÿ��Mesh��IndexAttribute��û�б�����ΪEBO

		if (mSortObject) {
			mRenderList->sort();
		}

		//make frame data
		mInfos->reset();

		//renderScene
		//���½�����һЩ������ϵѡ��û�й�ϵ��uniform����
		mRenderState->setupLights();
		

		//3 ��Ⱦ����
		//shadow
		mShadowMap->render(mRenderState, scene, camera);

		//drawBackground and clear 
		mBackground->render(mRenderList, scene);

		renderScene(mRenderList, scene, camera);

		return true;
	}

	void Renderer::swap() noexcept {
		mWindow->swap();
	}

	void Renderer::projectObject(const Object3D::Ptr& object, uint32_t groupOrder, bool sortObjects) noexcept {
		//��ǰ��Ҫ�����������壬����ǲ��ɼ����壬��ô��ͬ���ӽڵ�һ�𶼱�Ϊ���ɼ�״̬
		if (!object->mVisible) return;

		glm::vec4 toolVec(1.0f);

		//��object�����������жϣ����ҷֱ�����ͬ�Ĵ��� 
		if (object->mIsGroup) {
			auto group = std::static_pointer_cast<Group>(object);
			groupOrder = group->mGroupOrder;
		}
		else if (object->mIsLight) {
			auto light = std::static_pointer_cast<Light>(object);
			mRenderState->pushLight(light);
			if (light->mCastShadow) {
				mRenderState->pushShadow(light);
			}
		}
		//����ǿ���Ⱦ����
		else if (object->mIsRenderableObject) {
			//����
			if (object->mIsSkinnedMesh) {
				auto skinnedMesh = std::dynamic_pointer_cast<SkinnedMesh>(object);
				skinnedMesh->mSkeleton->update();
			}

			//�����Ҫ����Ⱦ�б��ж����������������Ҫ������z����ֵ(���ֵ��
			if (mSortObject) {
				toolVec = glm::vec4(object->getWorldPosition(), 1.0);
				toolVec = mCurrentViewMatrix * toolVec;
			}

			auto renderableObject = std::static_pointer_cast<RenderableObject>(object);

			//���ȶ�object����һ���Ӿ�����ò���
			if (mFrustum->intersectObject(renderableObject)) {

				//1 ��object geometry attribute���н��������
				auto geometry = mObjects->update(renderableObject);

				//2 �ó�material
				auto material = renderableObject->getMaterial();

				mRenderList->push(
					renderableObject,
					geometry,
					material,
					groupOrder,
					toolVec.z);
			}
		}

		auto children = object->getChildren();
		for (auto& child : children) {
			projectObject(child, groupOrder, sortObjects);
		}
	}

	void Renderer::renderScene(
		const DriverRenderList::Ptr& currentRenderList,
		const Scene::Ptr& scene,
		const Camera::Ptr& camera
	) noexcept {
		const auto opaqueObjects = currentRenderList->getOpaques();
		const auto transparentObjects = currentRenderList->getTransparents();

		//���ó�����ص�״̬�����������������չ�ܶೡ���������
		mRenderState->setupLightsView(camera);
		//scene viewport 
		mState->viewport(mViewport);

		if (!opaqueObjects.empty()) renderObjects(opaqueObjects, scene, camera);

		if (!transparentObjects.empty()) renderObjects(transparentObjects, scene, camera);

	}

	void Renderer::renderObjects(
		const std::vector<RenderItem::Ptr>& renderItems,
		const Scene::Ptr& scene,
		const Camera::Ptr& camera
	) noexcept {
		//�Ե�ǰĳһ����Ⱦ���е���Ⱦ���񣬽���һЩ��Ҫ��״̬����
		const auto overrideMaterial = scene->mIsScene ? scene->mOverrideMaterial : nullptr;

		for (const auto& renderItem : renderItems) {
			const auto object = renderItem->mObject;
			const auto geometry = renderItem->mGeometry;
			const auto material = overrideMaterial == nullptr ? renderItem->mMaterial : overrideMaterial;

			renderObject(object, scene, camera, geometry, material);
		}
	}

	void Renderer::renderObject(
		const RenderableObject::Ptr& object,
		const Scene::Ptr& scene,
		const Camera::Ptr& camera,
		const Geometry::Ptr& geometry,
		const Material::Ptr& material
	) noexcept {
		object->onBeforeRender(this, scene.get(), camera.get());

		//��Ҫ����Shader���е���Ҫuniform����
		object->updateModelViewMatrix(camera->getWorldMatrixInverse());
		object->updateNormalMatrix();

		//deal with double side
		renderBufferDirect(object, scene, camera, geometry, material);
	}

	void Renderer::renderBufferDirect(
		const RenderableObject::Ptr& object,
		const Scene::Ptr& scene,
		const Camera::Ptr& camera,
		const Geometry::Ptr& geometry,
		const Material::Ptr& material
	) noexcept {
		auto _scene = scene;
		if (_scene == nullptr) _scene = mDummyScene;

		auto index = geometry->getIndex();
		auto position = geometry->getAttribute("position");

		auto program = setProgram(camera, _scene, geometry, material, object);

		mState->setMaterial(material);

		//1 ���ɲ�����VAO
		//2 ���ð�״̬
		//3 ������VAO��״̬�Ļ���
		mBindingStates->setup(geometry, index);

		//draw
		if (index) {
			glDrawElements(toGL(material->mDrawMode), index->getCount(), toGL(index->getDataType()), 0);
		}
		else {
			glDrawArrays(toGL(material->mDrawMode), 0, position->getCount());
		}

	}

	//1 OpenGL ��һ��״̬��ϵͳ
	//2 ÿһ֡������ö��DrawCall
	//3 ÿһ��DrawCall��OpenGL������ݵ�ǰ�������õĸ���״̬�����л���
	//4 �����ǰ��һ֡����Ҫ�����������壬�͵õ�������DrawCall
	//5 ����ÿһ������֮ǰ���������OpenGL��״̬������ȷ�����ã�ʹ���ĸ�VAO��ʹ����һ��Shader������Program������ʹ�����֣�
	//6 ��OpenGL��API�������п����ģ������ܸ���״̬�͸��á�
	//7 ����״̬�������������嶼����ͬ����Program������Ҫ������Program�İ󶨣�ֻ��Ҫ����һ��UseProgram��������������
	//��Ҫ����
	//ƴװ���б��λ�����Ҫ��Uniforms��һ��outMap���棬Ȼ�����ͳһ�ĸ��²���
	DriverProgram::Ptr Renderer::setProgram(
		const Camera::Ptr& camera,
		const Scene::Ptr& scene,
		const Geometry::Ptr& geometry,
		const Material::Ptr& material,
		const RenderableObject::Ptr& object
	) noexcept {
		auto lights = mRenderState->mLights;

		//��־���Ƿ���Ҫ����һ���󶨵�Program
		bool needsProgramChange = false;

		//��Backend���棬��ȡ����ǰMaterial��DriverMaterial
		auto dMaterial = mMaterials->get(material);

		//����������һ��������߽��л��ƣ������һ֡�ĵ�һ�������Σ��ͱ���Ϊ������һ��DriverProgram
		//material��Version��ʼ��Ϊ1�� DriverMaterial��Version��ʼ��Ϊ0
		if (material->mVersion == dMaterial->mVersion) {

			//1 ������Version��ȣ�˵����ǰMaterial�����ǵ�һ�ν�������Materialһ���Ѿ�ӵ����һ��DriverProgram
			//2 ��Materialһ������ʹ��һ��Shader(vs/fs)
			//3 ���豾Material����һ֡û���õ�DiffuseMap, ������DriverProgram��ʱ�򣬾Ͳ���#define HAS_DIFFUSE_MAP
			// ��Shader��������Ĺ���DiffuseMap�Ĺ��ܾͲ��ᱻ�����ȥ
			//4 �����ڵ�ǰ��һ֡����Material���ո�������DiffuseMap,���Ծͱ�����Ҫһ��������DiffuseMap���ܵ�Shader
			// 
			// �ܽ᣺
			// ���µ��߼��������Ӱ��Shader���ܵĹؼ��������м�飬��һ���Ƿ����һ֡��Shader����״̬��ͬ
			//  
			//dMaterial update is in updateCommonMaterialProperties
			
			//���߲���Ȼ��������������һ������Texture�����ֱ仯�������shader�ṹ
			//�������������һ����Nullptr,�͵�������һ��Shader��
			if (material->mDiffuseMap != dMaterial->mDiffuseMap) {
				if (material->mDiffuseMap == nullptr || dMaterial->mDiffuseMap == nullptr) {
					needsProgramChange = true;
				}
			}

			if (material->mEnvMap != dMaterial->mEnvMap) {
				if (material->mEnvMap == nullptr || dMaterial->mEnvMap == nullptr) {
					needsProgramChange = true;
				}
			}

			if (material->mNormalMap != dMaterial->mNormalMap) {
				if (material->mNormalMap == nullptr || dMaterial->mNormalMap == nullptr) {
					needsProgramChange = true;
				}
			}

			if (material->mSpecularMap != dMaterial->mSpecularMap) {
				if (material->mSpecularMap == nullptr || dMaterial->mSpecularMap == nullptr) {
					needsProgramChange = true;
				}
			}

			if (dMaterial->mLightsStateVersion != lights->mState.mVersion) {
				needsProgramChange = true;
			}

			if (object->mIsSkinnedMesh != dMaterial->mSkinning) {
				dMaterial->mSkinning = object->mIsSkinnedMesh;
				needsProgramChange = true;
			}

			if (object->mIsSkinnedMesh) {
				auto skinnedMesh = std::dynamic_pointer_cast<SkinnedMesh>(object);
				if (skinnedMesh->mSkeleton->mBones.size() != dMaterial->mMaxBones) {
					needsProgramChange = true;
					dMaterial->mMaxBones = skinnedMesh->mSkeleton->mBones.size();
				}
			}
		}
		else {
			needsProgramChange = true;
			dMaterial->mVersion = material->mVersion;
		}

		//�����һ�ν���material����mCurrentProgramһ����nullptr
		auto dprogram = dMaterial->mCurrentProgram;
		if (needsProgramChange) {
			//���ɣ����߸���ԭ����Program�����Ҹ���ʹ�õ�Program
			dprogram = getProgram(material, scene, object);
		}

		bool refreshProgram = false;
		//useProgram���У���������˰󶨵�Program���͵ø���Uniform
		if (mState->useProgram(dprogram->mProgram)) {
			refreshProgram = true;
		}

		//----------------------------------���Ͼ���������Program�İ󶨹���-----------------------------------

		//----------------------------------չ������Uniforms���µĹ���-----------------------------------------

		//attention, ����map,��uniformsҲ��������˵��outMap��������UniformHandleMap
		auto uniforms = dMaterial->mUniforms;

		//DriverMaterial�������ǻ�����Ҫ��material����uniforms�����˸��´���
		DriverMaterials::refreshMaterialUniforms(uniforms, material);

		bool needsLights = materialNeedsLights(material);

		auto& lightUniforms = mRenderState->mLights->mState.mLightUniformHandles;

		//todo �൱�ڹ��ո���
		//�����material��Ҫ���գ��͵ðѹ�����ص�Uniforms�ϲ�����
		if (needsLights) {
			makeLightsNeedUpdate(lightUniforms);
			uniforms.insert(lightUniforms.begin(), lightUniforms.end());
		}

		//bones
		if (object->mIsSkinnedMesh) {
			auto skinnedMesh = std::dynamic_pointer_cast<SkinnedMesh>(object);
			auto skeleton = skinnedMesh->mSkeleton;
			uniforms.insert(skeleton->mUniforms.begin(), skeleton->mUniforms.end());
		}

		//ͨ�õ�һ�λ��Զ�����
		//uniforms��һ��unordered_map����ʱ��uniforms��û�м������µ�key-value���
		//��ʱ����������·�ʽ���ʣ�����ǿգ���ô�ͻ��Զ��½�һ����Ŀ��key-value�������ȥ��ֻ����uniformHandle��Ĭ��ֵ
		uniforms["modelViewMatrix"].mValue = object->getModelViewMatrix();
		uniforms["modelViewMatrix"].mNeedsUpdate = true;

		uniforms["normalMatrix"].mValue = object->getNormalMatrix();
		uniforms["normalMatrix"].mNeedsUpdate = true;

		uniforms["projectionMatrix"].mValue = camera->getProjectionMatrix();
		uniforms["projectionMatrix"].mNeedsUpdate = true;

		uniforms["modelMatrix"].mValue = object->getWorldMatrix();
		uniforms["modelMatrix"].mNeedsUpdate = true;

		DebugLog::getInstance()->beginUpLoad(material->getType());

		dprogram->uploadUniforms(uniforms, mTextures);

		DebugLog::getInstance()->end();

		return dprogram;
	}

	DriverProgram::Ptr Renderer::getProgram(
		const Material::Ptr& material,
		const Scene::Ptr& scene,
		const RenderableObject::Ptr& object
	) noexcept {
		DriverProgram::Ptr program = nullptr;

		auto dMaterial = mMaterials->get(material);
		auto lights = mRenderState->mLights;

		//����ǰ�ù���DriverProgramsȡ��������һ��map������
		auto& programs = dMaterial->mPrograms;

		//mPrograms��DriverPrograms��ͨ���·��Ľӿڣ����ɱ���RenderItem��Parameters
		auto parameters = mPrograms->getParameters(material, object, lights, mShadowMap);

		//ͨ��Parameters����һ����ϣֵ
		auto cacheKey = mPrograms->getProgramCacheKey(parameters);

		//���ù���DriverPrograms���棬���ҿ����Ƿ���������͵�DriverProgram,����ҵ���ʹ��
		auto pIter = programs.find(cacheKey);
		if (pIter != programs.end()) {
			dMaterial->mCurrentProgram = pIter->second;
			dMaterial->mUniforms = mPrograms->getUniforms(material);
			program = pIter->second;
		}
		else {
			//����Material�����Ͳ�ͬ��ȡ�����Ӧ������Uniform��map���ϣ���UniformHandleMap
			dMaterial->mUniforms = mPrograms->getUniforms(material);

			//���������ʹ�ù���DriverPrograms����û�ҵ�����ô�͵���������һ��
			program = mPrograms->acquireProgram(parameters, cacheKey);

			//�õ���һ��DriverProgram���͵ô���ڵ�ǰDriverMaterial������ʹ��Map����
			programs.insert(std::make_pair(cacheKey, program));

			dMaterial->mCurrentProgram = program;
		}

		updateCommonMaterialProperties(material, parameters);

		//update light state
		dMaterial->mNeedsLight = materialNeedsLights(material);
		dMaterial->mLightsStateVersion = lights->mState.mVersion;

		return program;
	}

	//ͳһ�˱�Material�����Ӧ��DriverMaterial�Ĺؼ��������Ӷ�����һ֡��ʱ�򣬲���needsProgramChange
	void Renderer::updateCommonMaterialProperties(
		const Material::Ptr& material,
		const DriverProgram::Parameters::Ptr& parameters) noexcept
	{
		auto dMaterial = mMaterials->get(material);

		dMaterial->mInstancing = parameters->mInstancing;
		dMaterial->mDiffuseMap = material->mDiffuseMap;
		dMaterial->mEnvMap = material->mEnvMap;
		dMaterial->mNormalMap = material->mNormalMap;
		dMaterial->mSpecularMap = material->mSpecularMap;
	}

	bool Renderer::materialNeedsLights(const Material::Ptr& material) noexcept {
		if (material->mIsMeshPhongMaterial) {
			return true;
		}

		return false;
	}

	void Renderer::makeLightsNeedUpdate(UniformHandleMap& lightsUniformMap) noexcept {
		for (auto& iter : lightsUniformMap) {
			iter.second.mNeedsUpdate = true;
		}
	}

	void Renderer::setSize(int width, int height) noexcept {
		mWidth = width;
		mHeight = height;

		mViewport.z = width;
		mViewport.w = height;

		mState->viewport(mViewport);

		//pay attention: we should deal with custom-renderTarget resizing in application
		if (mOnSizeCallback) {
			//�����ĵ��������ע��ص��Ľӿ�
			mOnSizeCallback(width, height);
		}
	}

	void Renderer::setRenderTarget(const RenderTarget::Ptr& renderTarget) noexcept {
		mCurrentRenderTarget = renderTarget;

		//���RenderTarget�ǿգ�˵��ʹ��Ĭ�ϵ�RenderTarget
		if (renderTarget == nullptr) {
			mState->bindFrameBuffer(0);
			return;
		}

		//�ó���DriverRenderTarget
		auto dRenderTarget = mRenderTargets->get(renderTarget);

		//�����ǰ��RenderTarget��û�б��������򽫴������񶪸�DriverTextures
		if (!dRenderTarget->mFrameBuffer) {
			mTextures->setupRenderTarget(renderTarget);
		}

		mState->bindFrameBuffer(dRenderTarget->mFrameBuffer);
	}

	RenderTarget::Ptr Renderer::getRenderTarget() const noexcept {
		return mCurrentRenderTarget;
	}

	void Renderer::setClearColor(float r, float g, float b, float a) noexcept {
		mState->setClearColor(r, g, b, a);
	}

	glm::vec4 Renderer::getClearColor() const noexcept {
		return mState->getClearColor();
	}

	void Renderer::clear(bool color, bool depth, bool stencil) noexcept {
		GLbitfield bits = 0;

		if (color) bits |= GL_COLOR_BUFFER_BIT;
		if (depth) bits |= GL_DEPTH_BUFFER_BIT;
		if (stencil) bits |= GL_STENCIL_BUFFER_BIT;

		glClear(bits);
	}

	void Renderer::enableShadow(bool enable) noexcept {
		mShadowMap->mEnabled = enable;
	}

	//Ϊ�β�ֱ��ʹ��driverWindow��set�������лص������أ�
	//�����С�ı仯��Ӱ������renderer��״̬,�����ӿ�viewport��Ҫ�������ñ仯
	void Renderer::setFrameSizeCallBack(const OnSizeCallback& callback) noexcept {
		mOnSizeCallback = callback;
	}

	void Renderer::setMouseMoveCallBack(const DriverWindow::MouseMoveCallback& callback) noexcept {
		mWindow->setMouseMoveCallBack(callback);
	}

	void Renderer::setMouseActionCallback(const DriverWindow::MouseActionCallback& callback) noexcept {
		mWindow->setMouseActionCallback(callback);
	}

	void Renderer::setKeyboardActionCallBack(const DriverWindow::KeyboardActionCallback& callback) noexcept {
		mWindow->setKeyboardActionCallBack(callback);
	}
}