#pragma once
#include "../global/base.h"
#include "../camera/camera.h"
#include "../core/object3D.h"
#include "../objects/mesh.h"
#include "../scene/scene.h"
#include "renderTarget.h"
#include "driver/driverAttributes.h"
#include "driver/driverBindingState.h"
#include "driver/driverPrograms.h"
#include "driver/driverWindow.h"
#include "driver/driverRenderList.h"
#include "driver/driverTextures.h"
#include "driver/driverObjects.h"
#include "driver/driverInfo.h"
#include "driver/driverState.h"
#include "driver/driverMaterials.h"
#include "driver/driverBackground.h"
#include "driver/driverRenderState.h"
#include "driver/driverRenderTargets.h"
#include "driver/driverShadowMap.h"
#include "../math/frustum.h"

namespace ff {

	class Renderer:public std::enable_shared_from_this<Renderer> {
	public:
	 	friend class DriverShadowMap;

		struct Descriptor {
			uint32_t mWidth{ 800 };
			uint32_t mHeight{ 600 };
		};

		using OnSizeCallback = std::function<void(int width, int height)>;

		using Ptr = std::shared_ptr<Renderer>;
		static Ptr create(const Descriptor& descriptor) {
			return std::make_shared <Renderer>(descriptor);
		}

		Renderer(const Descriptor& descriptor) noexcept;

		~Renderer() noexcept;

		bool render(Scene::Ptr scene, Camera::Ptr camera);

		void swap() noexcept;

		void setSize(int width, int height) noexcept;

		void setRenderTarget(const RenderTarget::Ptr& renderTarget) noexcept;

		void setFrameSizeCallBack(const OnSizeCallback& callback) noexcept;

		void setMouseMoveCallBack(const DriverWindow::MouseMoveCallback& callback) noexcept;

		void setMouseActionCallback(const DriverWindow::MouseActionCallback& callback) noexcept;

		void setKeyboardActionCallBack(const DriverWindow::KeyboardActionCallback& callback) noexcept;

		RenderTarget::Ptr getRenderTarget() const noexcept;

		void setClearColor(float r, float g, float b, float a) noexcept;

		glm::vec4 getClearColor() const noexcept;

		void enableShadow(bool enable) noexcept;

		void clear(bool color = true, bool depth = true, bool stencil = true) noexcept;

	public:
		//每一次绘制，是否需要自动擦除ColorBuffer
		bool mAutoClear{ true };

	private:

		// 1 object 当前需要被project的object，project：将层级架构的组织，展开成为列表
		// 2 groupOrder 当前其所处于的group的渲染优先级
		// 3 sortObjects 是否在渲染列表中，对item进行排序
		void projectObject(const Object3D::Ptr& object, uint32_t groupOrder, bool sortObjects) noexcept;

		//第一层级，在场景级别，进行一些状态的处理与设置，并且根据
		//实体/透明物体进行队列渲染-renderObjects
		void renderScene(const DriverRenderList::Ptr& currentRenderList, const Scene::Ptr& scene, const Camera::Ptr& camera) noexcept;

		//第二层级，在队列级别，进行一些状态的处理与设置
		//依次调用每个渲染单元，进入到renderObject
		void renderObjects(
			const std::vector<RenderItem::Ptr>& renderItems, 
			const Scene::Ptr& scene, 
			const Camera::Ptr& camera) noexcept;

		//第三层级，在单个渲染单元层面上，进行一些状态的处理与设置
		//并且调用跟API相关深重的renderBufferDirect
		void renderObject(
			const RenderableObject::Ptr& object,
			const Scene::Ptr& scene, 
			const Camera::Ptr& camera,
			const Geometry::Ptr& geometry,
			const Material::Ptr& material) noexcept;

		void renderBufferDirect(
			const RenderableObject::Ptr& object,
			const Scene::Ptr& scene,
			const Camera::Ptr& camera,
			const Geometry::Ptr& geometry,
			const Material::Ptr& material) noexcept;

		DriverProgram::Ptr setProgram(
			const Camera::Ptr& camera,
			const Scene::Ptr& scene, 
			const Geometry::Ptr& geometry,
			const Material::Ptr& material, 
			const RenderableObject::Ptr& object) noexcept;

		DriverProgram::Ptr getProgram(
			const Material::Ptr& material,
			const Scene::Ptr& scene,
			const RenderableObject::Ptr& object) noexcept;

		void updateCommonMaterialProperties(
			const Material::Ptr& material, 
			const DriverProgram::Parameters::Ptr& parameters) noexcept;

		bool materialNeedsLights(const Material::Ptr& material) noexcept;

		void makeLightsNeedUpdate(UniformHandleMap& lightsUniformMap) noexcept;

	private:
		int mWidth{ 800 };
		int mHeight{ 600 };

		OnSizeCallback mOnSizeCallback{ nullptr };

		bool		mSortObject{ true };

		glm::mat4	mCurrentViewMatrix = glm::mat4(1.0f);

		glm::vec4	mViewport{};

		RenderTarget::Ptr	mCurrentRenderTarget{ nullptr };

		DriverWindow::Ptr		mWindow{ nullptr };
		DriverRenderList::Ptr	mRenderList{ nullptr };
		DriverTextures::Ptr		mTextures{ nullptr };
		DriverAttributes::Ptr   mAttributes{ nullptr };
		DriverInfo::Ptr			mInfos{ nullptr };
		DriverState::Ptr		mState{ nullptr };
		DriverObjects::Ptr		mObjects{ nullptr };
		DriverGeometries::Ptr	mGeometries{ nullptr };
		DriverPrograms::Ptr		mPrograms{ nullptr };
		DriverBindingStates::Ptr	mBindingStates{ nullptr };
		DriverMaterials::Ptr	mMaterials{ nullptr };
		DriverBackground::Ptr	mBackground{ nullptr };
		DriverRenderState::Ptr	mRenderState{ nullptr };
		DriverRenderTargets::Ptr mRenderTargets{ nullptr };
		DriverShadowMap::Ptr	mShadowMap{ nullptr };

		Frustum::Ptr			mFrustum{ nullptr };

		//dummy objects
		Scene::Ptr				mDummyScene = Scene::create();
	};
}