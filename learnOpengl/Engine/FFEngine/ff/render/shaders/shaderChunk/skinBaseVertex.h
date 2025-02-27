#pragma once
#include "../../../global/base.h"

namespace ff {

	static const std::string skinBaseVertex =
		"#ifdef USE_SKINNING\n"\
		"	mat4 boneMatX = getBoneMatrix(skinIndex.x);\n"\
		"	mat4 boneMatY = getBoneMatrix(skinIndex.y);\n"\
		"	mat4 boneMatZ = getBoneMatrix(skinIndex.z);\n"\
		"	mat4 boneMatW = getBoneMatrix(skinIndex.w);\n"\
		"#endif\n"\
		"\n";
}