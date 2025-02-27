#include "shaderLib/meshBasicShader.h"
#include "shaderLib/cubeShader.h"
#include "shaderLib/meshPhongShader.h"
#include "shaderLib/depthShader.h"
#include "../../global/constant.h"
#include "uniformsLib.h"

namespace ff {

	struct Shader {
		//��ǰ���Shader������ı���Ҫ��UniformMap
		UniformHandleMap mUniformMap{};
		std::string mVertex;
		std::string mFragment;
	};

	//key-materialtypeName  value ->shader struct object
	static const std::unordered_map<std::string, Shader> ShaderLib = {
		{
			MaterialName::MeshBasicMaterial ,

			//��ʼ���б�ʽ��������һ��shader����
			{
				merge({
					UniformsLib.at("common")
				}),

				meshBasic::vertex, 
				meshBasic::fragment
			}
		},
		{
			MaterialName::MeshPhongMaterial ,
			{
				merge({
					UniformsLib.at("common"),
					UniformsLib.at("normalMap"),
					UniformsLib.at("specularMap")
				}),

				meshPhong::vertex,
				meshPhong::fragment
			}
		},
		{
			MaterialName::CubeMaterial ,
			{
				merge({
					UniformsLib.at("envMap")
				}),

				cube::vertex,
				cube::fragment
			}
		},
		{
			MaterialName::DepthMaterial ,
			{
				merge({}),

				depth::vertex,
				depth::fragment
			}
		}
	};

}