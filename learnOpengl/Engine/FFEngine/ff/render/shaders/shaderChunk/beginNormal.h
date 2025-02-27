#pragma once
#include "../../../global/base.h"

namespace ff {

	//�������Attribute���нӵ�һ���������У���������һ����ˮ�߶��±������мӹ�
	static const std::string beginNormal =
		"#ifdef HAS_NORMAL\n"\
		"	vec3 objectNormal = vec3(normal);\n"\
		"	#ifdef USE_TANGENT\n"\
		"		vec3 objectTangent = vec3(tangent);\n"\
		"		vec3 objectBitangent = vec3(bitangent);\n"\
		"	#endif\n"\
		"#endif\n";
}