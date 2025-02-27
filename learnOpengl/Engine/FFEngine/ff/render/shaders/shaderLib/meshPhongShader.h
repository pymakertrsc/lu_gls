#pragma once 
#include "../../../global/base.h"
#include "../shaderChunk/shaderChunk.h"

namespace ff {

	namespace meshPhong {

		//fs�������й��շ���λ�õȼ�����Ϣ�����������������ϵ֮�½���
		//��ȷÿ��shader������/���

		//vs���ܣ�������
		// 1 viewPosition:�����������������ϵ�µ�����
		// 2 fragNormal:�����������������ϵ�µ�Normal
		// 3 fragUV fragColor 
		// 
		static const std::string vertex =
			//���͸�fs�ı�������ʾ���������ϵ�µĶ���λ�ã���fs����ÿһ��fragment����õ����Լ������������ϵ�µ�λ��
			"out vec3 viewPosition;\n" +
			common +

			//����attribute������н�
			positionParseVertex +
			normalParseVertex +
			colorParseVertex +
			uvParseVertex +

			//�������ļ���
			uniformMatricesVertex +

			//��Ӱ��������
			shadowMapParseVertex + 
			skinningParseVertex + 

			"void main() {\n" +
			beginNormal + 
			
			//�������
			skinBaseVertex + 
			skinNormalVertex +

			//��normalת�������������ϵ
			normalDefaultVertex +
			//��fs���
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

			//ͨ�õ������ģ���޹صĽṹ������
			lightsParseBegin +

			//ֻ������Blinn-Phong����ģ�͵ļ���ģ��
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