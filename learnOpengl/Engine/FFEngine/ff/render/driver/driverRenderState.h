#pragma once
#include "../../global/base.h"
#include "../../global/constant.h"
#include "driverLights.h"
#include "../../lights/light.h"
#include "../../lights/lightShadow.h"
#include "../../lights/directionalLight.h"
#include "../../camera/camera.h"

namespace ff {

	//�洢�˹���Ӱ
	class DriverRenderState {
	public:
		using Ptr = std::shared_ptr<DriverRenderState>;
		static Ptr create() {
			return std::make_shared<DriverRenderState>();
		}

		DriverRenderState() noexcept;

		~DriverRenderState() noexcept;

		//ÿһ֡���������init����������е�����
		void init() noexcept;

		void setupLights() noexcept;

		void setupLightsView(const Camera::Ptr& camera) noexcept;

		void pushLight(const Light::Ptr& light) noexcept;

		void pushShadow(const Light::Ptr& shadowLight) noexcept;

	public:
		DriverLights::Ptr mLights = DriverLights::create();

		std::vector<Light::Ptr> mLightsArray{};//���г������еĹ�Դ
		std::vector<Light::Ptr> mShadowsArray{};//���г������п��Բ�����Ӱ�Ĺ�Դ
	};
}