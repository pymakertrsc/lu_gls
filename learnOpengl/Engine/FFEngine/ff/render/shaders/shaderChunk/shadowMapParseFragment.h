#pragma once
#include "../../../global/base.h"

namespace ff {

	static const std::string shadowMapParseFragment =
		"#ifdef USE_SHADOWMAP\n"\
		"	#if NUM_DIR_LIGHT_SHADOWS > 0\n"\
		"		uniform sampler2D directionalShadowMap[NUM_DIR_LIGHT_SHADOWS];\n"\
		"		in vec4 directionalShadowCoords[NUM_DIR_LIGHT_SHADOWS];\n"\
		"\n"\
		"		struct DirectionalLightShadow {\n"\
		"			float shadowRadius;\n"\
		"			float shadowBias;\n"\
		"			vec2 shadowMapSize;\n"\
		"		};\n"\
		"\n"\
		"		uniform DirectionalLightShadow directionalLightShadows[NUM_DIR_LIGHT_SHADOWS];\n"\
		"	#endif\n"\
		//return 1 if texture value is bigger than compare
		"	float texture2DCompare(sampler2D depths, vec2 uv, float compare) {\n"\
		"		return step(compare, unpackRGBAToDepth(texture2D(depths, uv)));\n"\
		"	}\n"\
		"\n"\
		"	float getShadow(sampler2D shadowMap, vec2 shadowMapSize, float shadowBias, float shadowRadius, vec4 shadowCoord) {\n"\
		"		float shadow = 1.0;\n"\
		"\n"\
		"		shadowCoord.xyz /= shadowCoord.w;\n"\
		"		shadowCoord.z += shadowBias;\n"\
		"\n"\
		"		bvec4 inFrustumXYVec = bvec4(shadowCoord.x >= 0.0, shadowCoord.x <=1.0, shadowCoord.y >= 0.0, shadowCoord.y <= 1.0);\n"\
		"		bool inFrustumXY = all(inFrustumXYVec);\n"\
		"\n"\
		"		bvec2 inFrusumZVec = bvec2(inFrustumXY, shadowCoord.z <= 1.0);\n"\
		"\n"\
		"		bool inFrustum = all(inFrusumZVec);\n"\
		"		if(inFrustum) {\n"\
		"			vec2 texelSize = vec2(1.0) / shadowMapSize;\n"\
		"			float dx0 = -texelSize.x * shadowRadius;\n"\
		"			float dy0 = -texelSize.y * shadowRadius;\n"\
		"			float dx1 = texelSize.x * shadowRadius;\n"\
		"			float dy1 = texelSize.y * shadowRadius;\n"\
		"			float dx2 = dx0 / 2.0;\n"\
		"			float dy2 = dy0 / 2.0;\n"\
		"			float dx3 = dx1 / 2.0;\n"\
		"			float dy3 = dy1 / 2.0;\n"\
		"\n"\
		"			shadow = (\n"\
		"				texture2DCompare(shadowMap, shadowCoord.xy + vec2(dx0, dy0), shadowCoord.z) +\n"\
		"				texture2DCompare(shadowMap, shadowCoord.xy + vec2(0.0, dy0), shadowCoord.z) +\n"\
		"				texture2DCompare(shadowMap, shadowCoord.xy + vec2(dx1, dy0), shadowCoord.z) +\n"\
		"				texture2DCompare(shadowMap, shadowCoord.xy + vec2(dx2, dy2), shadowCoord.z) +\n"\
		"				texture2DCompare(shadowMap, shadowCoord.xy + vec2(0.0, dy2), shadowCoord.z) +\n"\
		"				texture2DCompare(shadowMap, shadowCoord.xy + vec2(dx3, dy2), shadowCoord.z) +\n"\
		"				texture2DCompare(shadowMap, shadowCoord.xy + vec2(dx0, 0.0), shadowCoord.z) +\n"\
		"				texture2DCompare(shadowMap, shadowCoord.xy + vec2(dx2, 0.0), shadowCoord.z) +\n"\
		"				texture2DCompare(shadowMap, shadowCoord.xy, shadowCoord.z) +\n"\
		"				texture2DCompare(shadowMap, shadowCoord.xy + vec2(dx3, 0.0), shadowCoord.z) +\n"\
		"				texture2DCompare(shadowMap, shadowCoord.xy + vec2(dx1, 0.0), shadowCoord.z) +\n"\
		"				texture2DCompare(shadowMap, shadowCoord.xy + vec2(dx2, dy3), shadowCoord.z) +\n"\
		"				texture2DCompare(shadowMap, shadowCoord.xy + vec2(0.0, dy3), shadowCoord.z) +\n"\
		"				texture2DCompare(shadowMap, shadowCoord.xy + vec2(dx3, dy3), shadowCoord.z) +\n"\
		"				texture2DCompare(shadowMap, shadowCoord.xy + vec2(dx0, dy1), shadowCoord.z) +\n"\
		"				texture2DCompare(shadowMap, shadowCoord.xy + vec2(0.0, dy1), shadowCoord.z) +\n"\
		"				texture2DCompare(shadowMap, shadowCoord.xy + vec2(dx1, dy1), shadowCoord.z) \n"\
		"			) * (1.0 / 17.0);\n"\
		"		}\n"\
		"		return shadow;\n"\
		"	}\n"\
		"#endif\n"\
		"\n";
}