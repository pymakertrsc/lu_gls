#pragma once
#include "../../../global/base.h"

namespace ff {
	static const std::string uvParseVertex =
		"#ifdef HAS_UV\n"\
		"	layout(location = UV_LOCATION) in vec2 uv;\n"\
		"	out vec2 fragUV;\n"\
		"#endif\n"\
		"\n";
}