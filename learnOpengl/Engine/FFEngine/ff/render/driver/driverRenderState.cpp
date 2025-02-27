#include "driverRenderState.h"

namespace ff {

	DriverRenderState::DriverRenderState() noexcept {

	}

	DriverRenderState::~DriverRenderState() noexcept {}

	void DriverRenderState::init() noexcept {
		mLightsArray.clear();
		mShadowsArray.clear();
		mLights->init();
	}

	//������һЩ������ϵѡ���޹ص�uniform-color
	void DriverRenderState::setupLights() noexcept {
		mLights->setupLights(mLightsArray);
	}

	//����������ϵѡ���йص�uniforms-direction
	void DriverRenderState::setupLightsView(const Camera::Ptr& camera) noexcept {
		mLights->setupLightsView(mLightsArray, camera);
	}

	void DriverRenderState::pushLight(const Light::Ptr& light) noexcept {
		mLightsArray.push_back(light);
	}

	void DriverRenderState::pushShadow(const Light::Ptr& shadowLight) noexcept {
		mShadowsArray.push_back(shadowLight);
	}

}