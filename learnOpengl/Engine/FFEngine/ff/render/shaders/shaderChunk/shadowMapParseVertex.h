#pragma once
#include "../../../global/base.h"

namespace ff {

	static const std::string shadowMapParseVertex =
		"#ifdef USE_SHADOWMAP\n"\
		"	#if NUM_DIR_LIGHT_SHADOWS > 0\n"\
		"		uniform mat4 directionalShadowMatrix[NUM_DIR_LIGHT_SHADOWS];\n"\
		"		out vec4 directionalShadowCoords[NUM_DIR_LIGHT_SHADOWS];\n"\
		"\n"\
		"		struct DirectionalLightShadow {\n"\
		"			float shadowRadius;\n"\
		"			float shadowBias;\n"\
		"			vec2  shadowMapSize;\n"\
		"		};\n"\
		"\n"\
		"		uniform DirectionalLightShadow directionalLightShadows[NUM_DIR_LIGHT_SHADOWS];\n"\
		"	#endif\n"\
		"#endif\n"\
		"\n";
}
