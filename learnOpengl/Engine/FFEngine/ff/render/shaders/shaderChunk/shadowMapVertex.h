#pragma once
#include "../../../global/base.h"

namespace ff {

	static const std::string shadowMapVertex =
		"#ifdef USE_SHADOWMAP\n"\
		"	#if NUM_DIR_LIGHT_SHADOWS > 0\n"\
		"		vec4 shadowWorldPosition;\n"\
		"	#endif\n"\
		"\n"\
		"	#if NUM_DIR_LIGHT_SHADOWS > 0\n"\
		"	for(int i = 0;i < NUM_DIR_LIGHT_SHADOWS; i++) {\n"\
		"		shadowWorldPosition = worldPosition;\n"\
		"		directionalShadowCoords[i] = directionalShadowMatrix[i] * shadowWorldPosition;\n"\
		"	}\n"\
		"	#endif\n"\
		"\n"\
		"#endif\n"\
		"\n";
}