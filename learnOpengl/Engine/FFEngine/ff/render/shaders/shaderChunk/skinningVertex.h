#pragma once
#include "../../../global/base.h"

namespace ff {

	static const std::string skinningVertex =
		"#ifdef USE_SKINNING\n"\
		"	vec4 skinVertex = vec4(transformed, 1.0);\n"\
		"	vec4 skinned = vec4(0.0);\n"\
		"	skinned += boneMatX * skinVertex * skinWeight.x;\n"\
		"	skinned += boneMatY * skinVertex * skinWeight.y;\n"\
		"	skinned += boneMatZ * skinVertex * skinWeight.z;\n"\
		"	skinned += boneMatW * skinVertex * skinWeight.w;\n"\
		"\n"\
		"	transformed = skinned.xyz;\n"\
		"#endif\n"\
		"\n";
}