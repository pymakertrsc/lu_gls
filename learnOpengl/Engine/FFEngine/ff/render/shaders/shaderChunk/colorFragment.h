#pragma once
#include "../../../global/base.h"

namespace ff {
	static const std::string colorFragment =
		"#ifdef HAS_COLOR\n"\
		"	diffuseColor.rgb *= fragColor;\n"\
		"#endif\n"\
		"\n";
}