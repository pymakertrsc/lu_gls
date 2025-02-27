#pragma once 
#include "../../../global/base.h"
#include "../shaderChunk/shaderChunk.h"

namespace ff {

	namespace meshBasic {
		static const std::string vertex =
			common +
			positionParseVertex +
			normalParseVertex +
			colorParseVertex +
			uvParseVertex +
			uniformMatricesVertex +

			"void main() {\n" +
				beginNormal +
				normalDefaultVertex + 

				beginVertex +
				projectVertex +
				normalVertex +
				colorVertex +
				uvVertex +

			"}\n";

		static const std::string fragment =
			common + 
			normalParseFragment +
			colorParseFragment +
			uvParseFragment +
			diffuseMapParseFragment +

			"out vec4 fragmentColor;\n"+
			"uniform float opacity;\n"\
			"\n"\
			"void main() {\n" +
			"	vec4 diffuseColor = vec4(vec3(1.0), opacity);\n" +

				diffuseMapFragment +
				colorFragment + 

				"ReflectedLight reflectedLight = ReflectedLight(vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0));\n"\
				"reflectedLight.indirectDiffuse = vec3(1.0);\n"\
				"reflectedLight.indirectDiffuse *= diffuseColor.rgb;\n" +
				"\n" +
				"vec3 outgoingLight = reflectedLight.indirectDiffuse;\n" + 
				outputFragment +	
			"}\n";
	}
}