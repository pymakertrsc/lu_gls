#pragma once
#include "../../../global/base.h"

namespace ff {
	static const std::string lightsPhongParseFragment =
		"\n"\
		"struct BlinnPhongMaterial {\n"\
		"	vec3 diffuseColor;\n"\
		"	float specularShininess;\n"\
		"	float specularStrength;\n"\
		"};\n"\
		"\n"\
		"void RE_Direct_BlinnPhong(const in IncidentLight directLight, const in GeometricContext geometry, const in BlinnPhongMaterial material, inout ReflectedLight reflectedLight) {\n"\
		//diffuse computing
		"	float dotNL = saturate(dot(geometry.normal, -directLight.direction));\n"\
		"	reflectedLight.directDiffuse += material.diffuseColor * (directLight.color * dotNL + ambientLightColor);\n"\

		//specular computing
		"	vec3 halfWayDir = normalize(-directLight.direction + -geometry.viewDir);\n"\
		"	float dotNH = max(dot(geometry.normal, halfWayDir), 0.0);\n"\
		"	reflectedLight.directSpecular += pow(dotNH, material.specularShininess) * directLight.color * material.specularStrength;\n"\
		"}\n"\
		"\n"\
		"#define RE_Direct	RE_Direct_BlinnPhong\n"\
		"\n";
}