#pragma once 
#include "../../../global/base.h"
#include "../shaderChunk/shaderChunk.h"

namespace ff {

	namespace meshPhong {

		//fs当中所有光照方向、位置等几何信息，都是在摄像机坐标系之下进行
		//明确每个shader的输入/输出

		//vs功能：向后输出
		// 1 viewPosition:本顶点在摄像机坐标系下的坐标
		// 2 fragNormal:本顶点在摄像机坐标系下的Normal
		// 3 fragUV fragColor 
		// 
		static const std::string vertex =
			//输送给fs的变量，表示摄像机坐标系下的顶点位置，在fs里面每一个fragment都会得到他自己的摄像机坐标系下的位置
			"out vec3 viewPosition;\n" +
			common +

			//各种attribute的输入承接
			positionParseVertex +
			normalParseVertex +
			colorParseVertex +
			uvParseVertex +

			//各类矩阵的集合
			uniformMatricesVertex +

			//阴影与骨骼相关
			shadowMapParseVertex + 
			skinningParseVertex + 

			"void main() {\n" +
			beginNormal + 
			
			//骨骼相关
			skinBaseVertex + 
			skinNormalVertex +

			//将normal转化到摄像机坐标系
			normalDefaultVertex +
			//向fs输出
			normalVertex +

			beginVertex +
			skinningVertex + 
			projectVertex +
			colorVertex +
			uvVertex +
			"	viewPosition = mvPosition.xyz;\n"+
			worldPositionVertex + 
			shadowMapVertex +

			"}\n";

		static const std::string fragment =
			"in vec3 viewPosition;\n"\
			"uniform float shininess;\n"\
			"uniform float opacity;\n"\
			"\n"+
			common + 
			packing + 
			normalParseFragment +
			normalMapParseFragment +
			specularMapParseFragment +
			colorParseFragment +
			uvParseFragment +
			diffuseMapParseFragment +

			//通用的与光照模型无关的结构及函数
			lightsParseBegin +

			//只适用于Blinn-Phong光照模型的计算模块
			lightsPhongParseFragment +
			shadowMapParseFragment+
			"uniform mat4 modelViewMatrix;\n"\

			"out vec4 fragmentColor;\n" +

			"void main() {\n" +
			"vec4 diffuseColor = vec4(vec3(1.0), opacity);\n" +
			normalFragmentBegin + 
			normalFragmentMap + 
			diffuseMapFragment +
			colorFragment +
			specularMapFragment +
			"\n" +
			"ReflectedLight reflectedLight = ReflectedLight(vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0));\n"+

			//compute the light
			lightsPhongMaterial +
			lightsFragmentBegin +
			lightsFragmentEnd +
			"\n" +
			"vec3 outgoingLight = reflectedLight.directDiffuse + reflectedLight.directSpecular + reflectedLight.indirectDiffuse + reflectedLight.indirectSpecular;\n" +
			"\n" +
			outputFragment +
			"}\n";
	}
}