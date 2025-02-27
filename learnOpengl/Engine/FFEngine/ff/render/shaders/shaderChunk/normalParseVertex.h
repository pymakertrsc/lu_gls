#pragma once
#include "../../../global/base.h"

namespace ff {

	static const std::string normalParseVertex =
		//因为输入的模型顶点不一定有法线
		"#ifdef HAS_NORMAL\n"\
		"	layout(location = NORMAL_LOCATION) in vec3 normal;\n"\
		"	out vec3 fragNormal;\n"\
		"	#ifdef USE_TANGENT\n"\
		"		layout(location = TANGENT_LOCATION) in vec3 tangent;\n"\
		"		out vec3 fragTangent;\n"\
		"\n"\
		"		layout(location = BITANGENT_B_LOCATION) in vec3 bitangent;\n"\
		"		out vec3 fragBitangent;\n"\
		"	#endif\n"\
		"#endif\n"\
		"\n";
}