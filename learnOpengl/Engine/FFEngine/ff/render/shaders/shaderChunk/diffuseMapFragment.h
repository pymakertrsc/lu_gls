#pragma once
#include "../../../global/base.h"

namespace ff {
	static const std::string diffuseMapFragment =
		"#ifdef HAS_DIFFUSE_MAP\n"\
		"	diffuseColor.rgb = texture(diffuseMap, fragUV).rgb;\n"\
		"#endif\n"\
		"\n";
}