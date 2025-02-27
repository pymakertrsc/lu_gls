#pragma once
#include "../../global/base.h"
#include "../../global/constant.h"
#include "driverLights.h"
#include "../../lights/light.h"
#include "../../lights/lightShadow.h"
#include "../../lights/directionalLight.h"
#include "../../camera/camera.h"

namespace ff {

	//存储了光与影
	class DriverRenderState {
	public:
		using Ptr = std::shared_ptr<DriverRenderState>;
		static Ptr create() {
			return std::make_shared<DriverRenderState>();
		}

		DriverRenderState() noexcept;

		~DriverRenderState() noexcept;

		//每一帧，都会调用init，清理掉所有的数组
		void init() noexcept;

		void setupLights() noexcept;

		void setupLightsView(const Camera::Ptr& camera) noexcept;

		void pushLight(const Light::Ptr& light) noexcept;

		void pushShadow(const Light::Ptr& shadowLight) noexcept;

	public:
		DriverLights::Ptr mLights = DriverLights::create();

		std::vector<Light::Ptr> mLightsArray{};//所有场景当中的光源
		std::vector<Light::Ptr> mShadowsArray{};//所有场景当中可以产生阴影的光源
	};
}