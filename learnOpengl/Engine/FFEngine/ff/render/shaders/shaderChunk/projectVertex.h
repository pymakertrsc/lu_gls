#pragma once
#include "../../../global/base.h"

namespace ff {

	static const std::string projectVertex =
		"	vec4 mvPosition = vec4(transformed, 1.0);\n"\
		"	mvPosition = modelViewMatrix * mvPosition;\n"\
		"	gl_Position = projectionMatrix * mvPosition;\n";
}