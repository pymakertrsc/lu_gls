#pragma once
#include "../../../global/base.h"

namespace ff {

	static const std::string specularMapFragment =
		"float specularStrength = 1.0;\n"\
		"#ifdef USE_SPECULARMAP\n"\
		"	specularStrength = texture2D(specularMap, fragUV).r;\n"\
		"#endif\n"\
		"\n";
}