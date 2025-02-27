#pragma once
#include "../../../global/base.h"

namespace ff {

	static const std::string envMapCommonParseFragment =
		"#ifdef USE_ENVMAP\n"\
		"	uniform samplerCube envMap;\n"\
		"#endif\n"\
		"\n";
}