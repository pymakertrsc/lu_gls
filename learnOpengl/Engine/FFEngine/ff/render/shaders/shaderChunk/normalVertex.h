#pragma once
#include "../../../global/base.h"

namespace ff {

	static const std::string normalVertex =
		"#ifdef HAS_NORMAL\n"\
		"	fragNormal = normalize(transformedNormal);\n"\
		"	#ifdef USE_TANGENT\n"\
		"		fragTangent = normalize(transformedTangent);\n"\
		"		fragBitangent = normalize(transformedBitangent);\n"\
		"	#endif\n"\
		"#endif\n"\
		"\n";
}