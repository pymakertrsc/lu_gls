#pragma once
#include "../global/base.h"
#include "../camera/camera.h"
#include "../math/frustum.h"
#include "../render/renderTarget.h"

namespace ff {

	class Light;

	//描述了一盏光源，关于其阴影相关的各类参数
	class LightShadow {
	public:
		using Ptr = std::shared_ptr<LightShadow>;
		static Ptr create(const Camera::Ptr& camera) {
			return std::make_shared<LightShadow>(camera);
		}
 
		LightShadow(const Camera::Ptr& camera) noexcept;

		~LightShadow() noexcept;

		virtual void updateMatrices(const std::shared_ptr<Light>& light) noexcept;

		uint32_t getViewportCount() const noexcept;

		glm::vec4 getViewport(const uint32_t& index);

		Frustum::Ptr getFrustum() const noexcept;
 
	public:
		Camera::Ptr				mCamera{ nullptr };//放在光源位置，用于渲染深度贴图的摄像机
		float					mBias{ -0.003f };//为了防止ShadowAnce的深度偏移量
		float					mRadius{ 1.0f };//防止边缘过硬的时候，采用的米字型采样范围的大小

		glm::vec2				mMapSize = glm::vec2(512.0, 512.0);//ShadowMap分辨率的大小 

		//对于光源或者阴影，咱们系统实现了平行光系统，但是还没有实现点光源
		//真正想去了解点光源绘制的同学，可以看下threejs的引擎实现
		glm::vec2				mFrameExtent = glm::vec2(1.0, 1.0);
		std::vector<glm::vec4>	mViewports = { glm::vec4(0.0f, 0.0f, 1.0f, 1.0f) };

		RenderTarget::Ptr		mRenderTarget{ nullptr };//当前的ShadowMap所对应的渲染目标,ShadowMap就放在了他的ColorAttachment

		//1 将物体的顶点，从世界坐标系，转化到光源摄像机的投影坐标系内（NDC坐标组-还没有除以w）
		//2 projectionMatrix * viewMatrix（光源相机）
		//3 另外还得把NDC坐标从-1到1，转化为0-1
		glm::mat4				mMatrix = glm::mat4(1.0f);

	protected:
		//用来帮助光源摄像机进行视景体剪裁
		Frustum::Ptr			mFrustum = Frustum::create();
	};
}