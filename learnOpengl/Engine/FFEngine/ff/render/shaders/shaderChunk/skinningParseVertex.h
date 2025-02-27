#pragma once
#include "../../../global/base.h"

namespace ff {

	static const std::string skinningParseVertex =
		"#ifdef USE_SKINNING\n"\
		"	layout(location = SKINNING_INDICES_LOCATION) in vec4 skinIndex;\n"\
		"	layout(location = SKINNING_WEIGHTS_LOCATION) in vec4 skinWeight;\n"\
		"\n"\
		"	uniform mat4 boneMatrices[MAX_BONES];\n"\
		"	mat4 getBoneMatrix(const in float i) {\n"\
		"		return boneMatrices[int(i)];\n"\
		"	}\n"\
		"#endif\n"\
		"\n";
}