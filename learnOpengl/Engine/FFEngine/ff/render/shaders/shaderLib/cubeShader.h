#pragma once 
#include "../../../global/base.h"
#include "../shaderChunk/shaderChunk.h"

namespace ff {

	namespace cube {
		static const std::string vertex =
			common +
			positionParseVertex +
			"out vec3 uvw;\n"+
			uniformMatricesVertex +

			"void main() {\n" +
				"uvw = position;\n"+
				beginVertex +
				projectVertex +
				"gl_Position.z = gl_Position.w;\n"+
			"}\n";

		static const std::string fragment =
			envMapCommonParseFragment +
			"in vec3 uvw; \n"
			"out vec4 fragmentColor;\n" +

			"void main() {\n" +
				envMapFragment + 
			"	fragmentColor = envColor;\n" +
			"}";
	}
}