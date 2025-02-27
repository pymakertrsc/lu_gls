#pragma once 
#include "../../../global/base.h"
#include "../shaderChunk/shaderChunk.h"

namespace ff {

	namespace depth {
		static const std::string vertex =
			common +
			positionParseVertex +
			uniformMatricesVertex +
			skinningParseVertex +
			"out vec2 zw;\n"\

			"void main() {\n" +
			beginVertex +
			skinBaseVertex +
			skinningVertex +
			projectVertex +
			"zw = gl_Position.zw;\n"\
			"}\n";

		static const std::string fragment =
			common +
			packing+
			"in vec2 zw;\n"\
			"out vec4 fragmentColor;\n" +

			"void main() {\n" +
			"	float fragCoordZ = 0.5 * zw[0]/zw[1] + 0.5;\n"\
			"#ifdef DEPTH_PACKING_RGBA\n"\
			"	fragmentColor = packDepthToRGBA(fragCoordZ);\n"\
			"#else\n"\
			"	fragmentColor = vec4(vec3(fragCoordZ),1.0);\n"
			"#endif\n"\
			"}\n";
	}
}