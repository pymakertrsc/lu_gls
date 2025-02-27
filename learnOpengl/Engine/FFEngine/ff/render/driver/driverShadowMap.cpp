#include "driverShadowMap.h"
#include "driverObjects.h"
#include "driverRenderState.h"
#include "driverState.h"
#include "../renderer.h"

namespace ff {

	DriverShadowMap::DriverShadowMap(Renderer* renderer, const std::shared_ptr<DriverObjects>& objects, const std::shared_ptr<DriverState>& state) noexcept {
		mRenderer = renderer;
		mObjects = objects;
		mState = state;
	}

	DriverShadowMap::~DriverShadowMap() noexcept {}

	//任务
	// 1 填充Uniforms（shadowMapUniform， shadowMatrixUniform）
	// 2 生成每个光源的RenderTarget
	// 3 为每个光源渲染自己的ShadowMap
	//
	void DriverShadowMap::render(const std::shared_ptr<DriverRenderState>& renderState, const Scene::Ptr& scene, const Camera::Ptr& camera) noexcept {
		if (!mEnabled) return;

		//store old state
		auto currentRenderTarget = mRenderer->getRenderTarget();
		auto currentClearColor = mRenderer->getClearColor();

		mState->setBlending(BlendingType::NoBlending);
		mState->setDepth(true, true, CompareFunction::Less, 1.0);

		mRenderer->setClearColor(1.0, 1.0, 1.0, 1.0);

		//render depth map 

		glm::vec2 shadowMapSize = glm::vec2(0.0);
		glm::vec4 viewport = glm::vec4(0.0);
		glm::vec2 viewportSize = glm::vec2(0.0);
		glm::vec2 shadowFrameExtents = glm::vec2(1.0);//为了兼容点光源，但是在这里没有用
		Frustum::Ptr frustum = nullptr;

		//将会产生阴影的光源数组取出
		auto lights = renderState->mShadowsArray;

		//取出来光照系统的outMap
		auto& uniforms = renderState->mLights->mState.mLightUniformHandles;

		//clear shadow matrix array
		auto& shadowMapArray = uniforms["directionalShadowMap"];
		clearPureArrayUniform(std::any_cast<std::vector<Texture::Ptr>>(&shadowMapArray.mValue));

		auto& shadowMatrixArray = uniforms["directionalShadowMatrix"];
		clearPureArrayUniform(std::any_cast<std::vector<glm::mat4>>(&shadowMatrixArray.mValue));

		for (uint32_t i = 0; i < lights.size(); ++i) {
			auto light = lights[i];
			auto shadow = light->mShadow;

			if (shadow == nullptr) {
				std::cout << "Error: light has no shadow when rendering shadow map!" << std::endl;
				return;
			}

			shadowMapSize = shadow->mMapSize;
			shadowFrameExtents = shadow->mFrameExtent;//todo
			viewportSize = shadow->mMapSize;

			if (shadow->mRenderTarget == nullptr) {
				RenderTarget::Options options;
				options.mMinFilter = TextureFilter::NearestFilter;
				options.mMagFilter = TextureFilter::NearestFilter;
				options.mFormat = TextureFormat::RGBA;

				shadow->mRenderTarget = RenderTarget::create(shadowMapSize.x, shadowMapSize.y, options);
			}
			//give map to uniform handle
			pushPureArrayUniform(shadow->mRenderTarget->getTexture(), std::any_cast<std::vector<Texture::Ptr>>(&shadowMapArray.mValue));

			//开始向当前的shadowMap上面绘制，输出深度信息
			mRenderer->setRenderTarget(shadow->mRenderTarget);
			mRenderer->clear();

			//设置opengl渲染视口用的
			viewport = { 0.0, 0.0, viewportSize.x, viewportSize.y };

			mState->viewport(viewport);

			shadow->updateMatrices(light);

			//update uniform shadowmap matrix
			pushPureArrayUniform(shadow->mMatrix, std::any_cast<std::vector<glm::mat4>>(&shadowMatrixArray.mValue));

			frustum = shadow->getFrustum();

			renderObject(scene, camera, shadow->mCamera, light, frustum);
		}

		mRenderer->setRenderTarget(currentRenderTarget);
		mRenderer->setClearColor(currentClearColor.x, currentClearColor.y, currentClearColor.z, currentClearColor.w);
	}

	void DriverShadowMap::renderObject(
		const Object3D::Ptr& object,
		const Camera::Ptr& camera,
		const Camera::Ptr& shadowCamera,
		const Light::Ptr& light,
		const Frustum::Ptr& frustum) noexcept
	{
		if (!object->mVisible) return;

		if (object->mIsRenderableObject) {
			auto renderableObject = std::static_pointer_cast<RenderableObject>(object);

			if (renderableObject->mCastShadow && frustum->intersectObject(renderableObject)) {
				renderableObject->updateModelViewMatrix(shadowCamera->getWorldMatrixInverse());

				auto geometry = mObjects->update(renderableObject);

				//所有物体统一使用默认的深度材质
				auto material = mDefaultDepthMaterial;

				mRenderer->renderBufferDirect(renderableObject, nullptr, shadowCamera, geometry, material);
			}
			else {
				std::cout << std::endl;
			}

		}

		auto children = object->getChildren();
		for (const auto& child : children) {
			renderObject(child, camera, shadowCamera, light, frustum);
		}
	}
}