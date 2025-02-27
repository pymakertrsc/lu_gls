#pragma once
#include "../../../global/base.h"

namespace ff {
	static const std::string lightsFragmentBegin =
		"GeometricContext geometry;\n"\
		"\n"\
		"geometry.position = viewPosition;\n"\
		"geometry.normal = normal;\n"\
		"geometry.viewDir = normalize(viewPosition);\n"\
		"\n"\
		"IncidentLight directLight;\n"\
		"\n"\
		"#if (NUM_DIR_LIGHTS > 0) && defined(RE_Direct)\n"\
		"	DirectionalLight directionalLight;\n"\
		"	#if defined(USE_SHADOWMAP) && NUM_DIR_LIGHT_SHADOWS >0\n"\
		"		DirectionalLightShadow directionalLightShadow;\n"\
		"	#endif\n"\
		"	for(int i = 0; i < NUM_DIR_LIGHTS;i++) {\n"\
		"		directionalLight = directionalLights[i];\n"\
		"		getDirectionalLightInfo(directionalLight, geometry, directLight);\n"\
		"\n"\
		"	#if defined(USE_SHADOWMAP) && NUM_DIR_LIGHT_SHADOWS > 0\n"\
		"		if(i < NUM_DIR_LIGHT_SHADOWS) {\n"\
		"			directionalLightShadow = directionalLightShadows[i];\n"\
		"			directLight.color *= getShadow(directionalShadowMap[i], directionalLightShadow.shadowMapSize, directionalLightShadow.shadowBias, directionalLightShadow.shadowRadius, directionalShadowCoords[i]);\n"\
		"		}\n"\
		"	#endif\n"\
		"		RE_Direct(directLight, geometry, material, reflectedLight);\n"\
		"	}\n"\
		"#endif\n"\
		"\n";

}