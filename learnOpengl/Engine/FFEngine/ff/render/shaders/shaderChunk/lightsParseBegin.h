#pragma once
#include "../../../global/base.h"

namespace ff {
	static const std::string lightsParseBegin =
		"uniform vec3 ambientLightColor;\n"\
		"\n"\
		"#if NUM_DIR_LIGHTS > 0\n"\
		"	struct DirectionalLight {\n"\
		"		vec3 direction;\n"\
		"		vec3 color;\n"\
		"	};\n"\
		"\n"\
		"	uniform DirectionalLight directionalLights[NUM_DIR_LIGHTS];\n"\
		"\n"\
		"	void getDirectionalLightInfo(const in DirectionalLight directionalLight, const in GeometricContext geometry, out IncidentLight light) {\n"\
		"		light.color = directionalLight.color;\n"\
		"		light.direction = directionalLight.direction;\n"\
		"	}\n"\
		"#endif\n"\
		"\n";
}