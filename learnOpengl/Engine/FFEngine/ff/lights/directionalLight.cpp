#pragma once
#include "directionalLight.h"

namespace ff {

	DirectionalLight::DirectionalLight() noexcept {
		mIsDirectionalLight = true;
		mShadow = DirectionalLightShadow::create();
	}

	DirectionalLight::~DirectionalLight() noexcept {}
}