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
		//处理窗体消息，并且判断是否继续render
		if (!mWindow->processEvent()) {
			return false;
		}

		if (scene == nullptr) { scene = mDummyScene; }

		//1 更新场景数据
		scene->updateWorldMatrix(true, true);
		camera->updateWorldMatrix(true, true);

		auto projectionMatrix = camera->getProjectionMatrix();
		auto cameraInverseMatrix = camera->getWorldMatrixInverse();

		mCurrentViewMatrix = projectionMatrix * cameraInverseMatrix;
		mFrustum->setFromProjectionMatrix(mCurrentViewMatrix);

		//2 提取渲染数据，构成渲染列表与状态
		mRenderState->init();
		mRenderList->init();

		//scene当中的数据都是层级架构的树状数据，从这个结构，解析为一个线性列表
		projectObject(scene, 0, mSortObject);

		//调用完毕projectObject之后，所有可渲染物体&在视景体范围内的，都已经被压入到了RenderList当中
		mRenderList->finish();

		//经过上述projectObject的流程，任何一个我们使用到的Attribute都已经成功的被解析成为了一个VBO
		//在上述流程中，每个Mesh的IndexAttribute并没有被解析为EBO

		if (mSortObject) {
			mRenderList->sort();
		}

		//make frame data
		mInfos->reset();

		//renderScene
		//更新建设了一些与坐标系选择没有关系的uniform内容
		mRenderState->setupLights();
		

		//3 渲染场景
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
		//当前需要被解析的物体，如果是不可见物体，那么连同其子节点一起都变为不可见状态
		if (!object->mVisible) return;

		glm::vec4 toolVec(1.0f);

		//对object进行了类型判断，并且分别做不同的处理 
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
		//如果是可渲染物体
		else if (object->mIsRenderableObject) {
			//骨骼
			if (object->mIsSkinnedMesh) {
				auto skinnedMesh = std::dynamic_pointer_cast<SkinnedMesh>(object);
				skinnedMesh->mSkeleton->update();
			}

			//如果需要在渲染列表当中对物体进行排序，则需要计算其z坐标值(深度值）
			if (mSortObject) {
				toolVec = glm::vec4(object->getWorldPosition(), 1.0);
				toolVec = mCurrentViewMatrix * toolVec;
			}

			auto renderableObject = std::static_pointer_cast<RenderableObject>(object);

			//首先对object进行一次视景体剪裁测试
			if (mFrustum->intersectObject(renderableObject)) {

				//1 对object geometry attribute进行解析与更新
				auto geometry = mObjects->update(renderableObject);

				//2 拿出material
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

		//设置场景相关的状态，可以在这里继续扩展很多场景相关设置
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
		//对当前某一个渲染队列的渲染任务，进行一些必要的状态设置
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

		//需要传入Shader当中的重要uniform变量
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

		//1 生成并管理VAO
		//2 设置绑定状态
		//3 负责了VAO绑定状态的缓存
		mBindingStates->setup(geometry, index);

		//draw
		if (index) {
			glDrawElements(toGL(material->mDrawMode), index->getCount(), toGL(index->getDataType()), 0);
		}
		else {
			glDrawArrays(toGL(material->mDrawMode), 0, position->getCount());
		}

	}

	//1 OpenGL 是一个状态机系统
	//2 每一帧都会调用多个DrawCall
	//3 每一次DrawCall，OpenGL都会根据当前我们设置的各类状态，进行绘制
	//4 如果当前这一帧，需要绘制三个物体，就得调用三次DrawCall
	//5 绘制每一个物体之前，都必须对OpenGL的状态进行正确的设置（使用哪个VAO，使用哪一套Shader产生的Program，管线使用哪种）
	//6 对OpenGL的API调用是有开销的，所以能复用状态就复用。
	//7 复用状态：假设三个物体都用了同样的Program，则不需要做三次Program的绑定，只需要调用一次UseProgram，绘制三个物体
	//重要任务：
	//拼装所有本次绘制需要的Uniforms到一个outMap里面，然后进行统一的更新操作
	DriverProgram::Ptr Renderer::setProgram(
		const Camera::Ptr& camera,
		const Scene::Ptr& scene,
		const Geometry::Ptr& geometry,
		const Material::Ptr& material,
		const RenderableObject::Ptr& object
	) noexcept {
		auto lights = mRenderState->mLights;

		//标志着是否需要更换一个绑定的Program
		bool needsProgramChange = false;

		//从Backend里面，获取到当前Material的DriverMaterial
		auto dMaterial = mMaterials->get(material);

		//如果本物体第一次送入管线进行绘制，比如第一帧的第一个三角形，就必须为其生成一个DriverProgram
		//material的Version初始化为1， DriverMaterial的Version初始化为0
		if (material->mVersion == dMaterial->mVersion) {

			//1 当二者Version相等，说明当前Material并不是第一次解析，则本Material一定已经拥有了一个DriverProgram
			//2 本Material一定正在使用一组Shader(vs/fs)
			//3 假设本Material在上一帧没有用到DiffuseMap, 在生成DriverProgram的时候，就不会#define HAS_DIFFUSE_MAP
			// 则Shader代码里面的关于DiffuseMap的功能就不会被编译进去
			//4 假设在当前这一帧，本Material被刚刚设置了DiffuseMap,所以就必须需要一个激活了DiffuseMap功能的Shader
			// 
			// 总结：
			// 以下的逻辑，都会对影响Shader功能的关键变量进行检查，看一看是否跟上一帧的Shader代码状态相同
			//  
			//dMaterial update is in updateCommonMaterialProperties
			
			//二者不相等会有两种情况，其一更换了Texture，这种变化不会更改shader结构
			//其二，二者其中一个是Nullptr,就得重新找一个Shader组
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

		//如果第一次解析material，则mCurrentProgram一定是nullptr
		auto dprogram = dMaterial->mCurrentProgram;
		if (needsProgramChange) {
			//生成，或者复用原来的Program，并且给出使用的Program
			dprogram = getProgram(material, scene, object);
		}

		bool refreshProgram = false;
		//useProgram当中，如果更换了绑定的Program，就得更新Uniform
		if (mState->useProgram(dprogram->mProgram)) {
			refreshProgram = true;
		}

		//----------------------------------以上就是做完了Program的绑定工作-----------------------------------

		//----------------------------------展开对于Uniforms更新的工作-----------------------------------------

		//attention, 拷贝map,本uniforms也就是我们说的outMap，类型是UniformHandleMap
		auto uniforms = dMaterial->mUniforms;

		//DriverMaterial根据我们绘制需要的material，对uniforms进行了更新处理
		DriverMaterials::refreshMaterialUniforms(uniforms, material);

		bool needsLights = materialNeedsLights(material);

		auto& lightUniforms = mRenderState->mLights->mState.mLightUniformHandles;

		//todo 相当于光照更新
		//如果本material需要光照，就得把光照相关的Uniforms合并进来
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

		//通用第一次会自动加入
		//uniforms是一个unordered_map，此时的uniforms还没有加入如下的key-value组合
		//此时如果按照如下方式访问，如果是空，那么就会自动新建一个条目（key-value）插入进去，只不过uniformHandle是默认值
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

		//将以前用过的DriverPrograms取出来，是一个map的引用
		auto& programs = dMaterial->mPrograms;

		//mPrograms是DriverPrograms，通过下方的接口，生成本个RenderItem的Parameters
		auto parameters = mPrograms->getParameters(material, object, lights, mShadowMap);

		//通过Parameters计算一个哈希值
		auto cacheKey = mPrograms->getProgramCacheKey(parameters);

		//从用过的DriverPrograms里面，找找看，是否有这个类型的DriverProgram,如果找到就使用
		auto pIter = programs.find(cacheKey);
		if (pIter != programs.end()) {
			dMaterial->mCurrentProgram = pIter->second;
			dMaterial->mUniforms = mPrograms->getUniforms(material);
			program = pIter->second;
		}
		else {
			//根据Material的类型不同，取得其对应的特殊Uniform的map集合，即UniformHandleMap
			dMaterial->mUniforms = mPrograms->getUniforms(material);

			//如果在曾经使用过的DriverPrograms里面没找到，那么就得重新生成一个
			program = mPrograms->acquireProgram(parameters, cacheKey);

			//得到了一个DriverProgram，就得存放在当前DriverMaterial的曾经使用Map当中
			programs.insert(std::make_pair(cacheKey, program));

			dMaterial->mCurrentProgram = program;
		}

		updateCommonMaterialProperties(material, parameters);

		//update light state
		dMaterial->mNeedsLight = materialNeedsLights(material);
		dMaterial->mLightsStateVersion = lights->mState.mVersion;

		return program;
	}

	//统一了本Material跟其对应的DriverMaterial的关键变量，从而在下一帧的时候，不会needsProgramChange
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
			//真正的调用了外界注册回调的接口
			mOnSizeCallback(width, height);
		}
	}

	void Renderer::setRenderTarget(const RenderTarget::Ptr& renderTarget) noexcept {
		mCurrentRenderTarget = renderTarget;

		//如果RenderTarget是空，说明使用默认的RenderTarget
		if (renderTarget == nullptr) {
			mState->bindFrameBuffer(0);
			return;
		}

		//拿出来DriverRenderTarget
		auto dRenderTarget = mRenderTargets->get(renderTarget);

		//如果当前的RenderTarget还没有被创建，则将创建任务丢给DriverTextures
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

	//为何不直接使用driverWindow的set函数进行回调设置呢？
	//窗体大小的变化会影响咱们renderer的状态,比如视口viewport需要跟随设置变化
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