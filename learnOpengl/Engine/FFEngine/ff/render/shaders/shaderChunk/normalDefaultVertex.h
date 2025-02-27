#pragma once
#include "../../../global/base.h"

namespace ff {

	static const std::string normalDefaultVertex =
		"#ifdef HAS_NORMAL\n"\
		"	vec3 transformedNormal = normalMatrix * objectNormal;\n"\
		"\n"\
		"	#ifdef USE_TANGENT\n"\
		//because tangent is the base vector 
		"		vec3 transformedTangent = (modelViewMatrix * vec4(objectTangent, 0.0)).xyz;\n"\
		"		vec3 transformedBitangent = (modelViewMatrix * vec4(objectBitangent, 0.0)).xyz;\n"\
		"	#endif\n"\
		"#endif\n"\
		"\n";
}