#pragma once
#include "../../../global/base.h"

namespace ff {
	static const std::string colorVertex =
		"#ifdef HAS_COLOR\n"\
		"	fragColor = color;\n"\
		"#endif\n"\
		"\n";
}