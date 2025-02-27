#include "ambientLight.h"

namespace ff {

	AmbientLight::AmbientLight() noexcept {
		mIsAmbientLight = true;
		mIntensity = 0.1f;
	}

	AmbientLight::~AmbientLight() noexcept {}
}