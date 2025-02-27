#pragma once
#include "../../../global/base.h"

namespace ff {

	//将输入的Attribute，承接到一个变量当中，方便下面一条流水线对新变量进行加工
	static const std::string beginNormal =
		"#ifdef HAS_NORMAL\n"\
		"	vec3 objectNormal = vec3(normal);\n"\
		"	#ifdef USE_TANGENT\n"\
		"		vec3 objectTangent = vec3(tangent);\n"\
		"		vec3 objectBitangent = vec3(bitangent);\n"\
		"	#endif\n"\
		"#endif\n";
}