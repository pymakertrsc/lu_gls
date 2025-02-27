#include "directionalLightShadow.h"
#include "../camera/orthographicCamera.h"

namespace ff {

	DirectionalLightShadow::DirectionalLightShadow()noexcept :
		LightShadow(OrthographicCamera::create(-10.0f, 10.0f, -10.0f, 10.0f, -100.0f, 100.0f)) {}

	DirectionalLightShadow::~DirectionalLightShadow()noexcept {}
}