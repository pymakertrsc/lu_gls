#pragma once
#include "../../../global/base.h"

namespace ff {

	static const std::string packing =
		"const float PackUpscale = 256. / 255.; // fraction -> 0..1 (including 1)\n"\
		"const float UnpackDownscale = 255. / 256.; // 0..1 -> fraction (excluding 1)\n"\
		"const vec3 PackFactors = vec3(256. * 256. * 256., 256. * 256., 256.);\n"\
		"const vec4 UnpackFactors = UnpackDownscale / vec4(PackFactors, 1.);\n"\

		"const float ShiftRight8 = 1. / 256.; \n"\

		"vec4 packDepthToRGBA(const in float v) {\n"\
		"	vec4 r = vec4(fract(v * PackFactors), v);\n"\
		"	r.yzw -= r.xyz * ShiftRight8; // tidy overflow\n"\
		"	return r * PackUpscale;\n"\
		"}\n"\

		"float unpackRGBAToDepth(const in vec4 v) {\n"\
		"	return dot(v, UnpackFactors);\n"\
		"}\n";
}