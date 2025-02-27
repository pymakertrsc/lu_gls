#pragma once
#include "../../../global/base.h"

namespace ff {
	static const std::string uvParseFragment =
		"#ifdef HAS_UV\n"\
		"	in vec2 fragUV;\n"\
		"#endif\n"\
		"\n";
}