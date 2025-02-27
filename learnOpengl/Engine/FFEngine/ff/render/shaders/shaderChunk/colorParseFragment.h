#pragma once
#include "../../../global/base.h"

namespace ff {
	static const std::string colorParseFragment =
		"#ifdef HAS_COLOR\n"\
		"	in vec3 fragColor;\n"\
		"#endif\n"\
		"\n";
}
