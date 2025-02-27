#pragma once
#include "../../../global/base.h"

namespace ff {

	static const std::string specularMapParseFragment =
		"#ifdef USE_SPECULARMAP\n"\
		"	uniform sampler2D specularMap;\n"\
		"#endif\n"\
		"\n";
}