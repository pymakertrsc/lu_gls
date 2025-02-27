#pragma once
#include "../../../global/base.h"

namespace ff {

	static const std::string diffuseMapParseFragment =
		"#ifdef HAS_DIFFUSE_MAP\n"\
		"	uniform sampler2D diffuseMap;\n"\
		"#endif\n"\
		"\n";
}