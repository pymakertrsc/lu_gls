#pragma once
#include "../../../global/base.h"

namespace ff {

	static const std::string skinNormalVertex =
		"#ifdef USE_SKINNING\n"\
		"	mat4 skinMatrix = mat4(0.0);\n"\
		"	skinMatrix += boneMatX * skinWeight.x;\n"\
		"	skinMatrix += boneMatY * skinWeight.y;\n"\
		"	skinMatrix += boneMatZ * skinWeight.z;\n"\
		"	skinMatrix += boneMatW * skinWeight.w;\n"\
		"\n"\
		"	objectNormal = vec4(skinMatrix * vec4(objectNormal, 0.0)).xyz;\n"\
		"	#ifdef USE_TANGENT\n"\
		"		objectTangent = vec4(skinMatrix * vec4(objectTangent, 0.0)).xyz;\n"\
		"		objectBitangent = vec4(skinMatrix * vec4(objectBitangent, 0.0)).xyz;\n"\
		"	#endif\n"\
		"#endif\n"\
		"\n";
}