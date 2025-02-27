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

	//更新了一些与坐标系选择无关的uniform-color
	void DriverRenderState::setupLights() noexcept {
		mLights->setupLights(mLightsArray);
	}

	//更新与坐标系选择有关的uniforms-direction
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