#pragma once
#include "../../../global/base.h"

namespace ff {
	static const std::string normalMapParseFragment =
		"#ifdef USE_NORMALMAP\n"\
		"	uniform sampler2D normalMap;\n"\
		"#endif\n"\
		"\n";
}