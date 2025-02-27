#include "driverPrograms.h"
#include "../../tools/identity.h"
#include "../shaders/shaderLib.h"
#include "../../material/depthMaterial.h"
#include "../../log/debugLog.h"
#include "../../objects/skinnedMesh.h"

namespace ff {

	//1 ��Ҫ�Ժܶ๦�ܽ���#define�Ĳ������Ӷ���������Щ�����
	//2 ռλ�ַ������滻,����POSITION_LOCATIONռλ�ַ����滻Ϊ0
	DriverProgram::DriverProgram(const Parameters::Ptr& parameters) noexcept {
		mID = Identity::generateID();

		//1 shader�汾�ַ���
		std::string versionString = "#version 330 core\n";

		//2 shader��չ�ַ��� 
		std::string extensionString = getExtensionString();

		//3 prefix�ַ�����define�ĸ������������뵽prefix���У��Ӷ������������뵱����Щ���ܿ��Ա���
		std::string prefixVertex;
		std::string prefixFragment;

		prefixVertex.append(parameters->mHasNormal ? "#define HAS_NORMAL\n" : "");
		prefixVertex.append(parameters->mHasUV ? "#define HAS_UV\n" : "");
		prefixVertex.append(parameters->mHasColor ? "#define HAS_COLOR\n" : "");

		prefixVertex.append(parameters->mShadowMapEnabled ? "#define USE_SHADOWMAP\n" : "");
		prefixVertex.append(parameters->mSkinning ? "#define USE_SKINNING\n" : "");
		prefixVertex.append(parameters->mSkinning ? std::string("#define MAX_BONES ") + std::to_string(parameters->mMaxBones) + "\n" : "");
		prefixVertex.append(parameters->mUseNormalMap ? "#define USE_NORMALMAP\n" : "");
		prefixVertex.append(parameters->mUseTangent ? "#define USE_TANGENT\n" : "");

		prefixFragment.append(parameters->mHasNormal ? "#define HAS_NORMAL\n" : "");
		prefixFragment.append(parameters->mHasUV ? "#define HAS_UV\n" : "");
		prefixFragment.append(parameters->mHasColor ? "#define HAS_COLOR\n" : "");
		prefixFragment.append(parameters->mHasDiffuseMap ? "#define HAS_DIFFUSE_MAP\n" : "");
		prefixFragment.append(parameters->mHasEnvCubeMap ? "#define USE_ENVMAP\n" : "");
		prefixFragment.append(parameters->mHasSpecularMap ? "#define USE_SPECULARMAP\n" : "");

		prefixFragment.append(parameters->mShadowMapEnabled ? "#define USE_SHADOWMAP\n" : "");
		prefixFragment.append(parameters->mDepthPacking == DepthMaterial::RGBADepthPacking ? "#define DEPTH_PACKING_RGBA\n" : "");
		prefixFragment.append(parameters->mUseNormalMap ? "#define USE_NORMALMAP\n" : "");
		prefixFragment.append(parameters->mUseTangent ? "#define USE_TANGENT\n" : "");

		//4 ��parameters����ȡ����vs/fs�������ܴ���
		auto vertexString = parameters->mVertex;
		auto fragmentString = parameters->mFragment;

		replaceAttributeLocations(vertexString);
		replaceAttributeLocations(fragmentString);

		replaceLightNumbers(vertexString, parameters);
		replaceLightNumbers(fragmentString, parameters);

		//�汾����չ��ǰ׺prefix��define���ֹ��ܵĿ�����+ ����shader
		vertexString = versionString + extensionString + prefixVertex + vertexString;
		fragmentString = versionString + extensionString + prefixFragment + fragmentString;

		auto vertex = vertexString.c_str();
		auto fragment = fragmentString.c_str();

		std::cout << vertex << std::endl;
		std::cout << fragment << std::endl;
		std::cout << "----------------------------------------------------" << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;

		//shader�ı���������
		uint32_t vertexID = 0, fragID = 0;
		char infoLog[512];
		int  successFlag = 0;

		vertexID = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexID, 1, &vertex, NULL);
		glCompileShader(vertexID);

		//��ȡ������Ϣ
		glGetShaderiv(vertexID, GL_COMPILE_STATUS, &successFlag);
		if (!successFlag)
		{
			glGetShaderInfoLog(vertexID, 512, NULL, infoLog);
			std::cout << infoLog << std::endl;
		}

		fragID = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragID, 1, &fragment, NULL);
		glCompileShader(fragID);

		glGetShaderiv(fragID, GL_COMPILE_STATUS, &successFlag);
		if (!successFlag)
		{
			glGetShaderInfoLog(fragID, 512, NULL, infoLog);
			std::cout << infoLog << std::endl;
		}

		//����
		mProgram = glCreateProgram();
		glAttachShader(mProgram, vertexID);
		glAttachShader(mProgram, fragID);
		glLinkProgram(mProgram);

		glGetProgramiv(mProgram, GL_LINK_STATUS, &successFlag);
		if (!successFlag)
		{
			glGetProgramInfoLog(mProgram, 512, NULL, infoLog);
			std::cout << infoLog << std::endl;
		}
		glDeleteShader(vertexID);
		glDeleteShader(fragID);

		DebugLog::getInstance()->beginPrintUniformInfo(parameters->mShaderID);
		mUniforms = DriverUniforms::create(mProgram);
		DebugLog::getInstance()->end();
	}

	DriverProgram::~DriverProgram() noexcept {
		glDeleteProgram(mProgram);
	}

	void DriverProgram::replaceAttributeLocations(std::string& shader) noexcept {
		//1 ͨ��������ʽ��ƥ����Ӧ��ռλ�����������ƥ��  POSITION_LOCATION
		//2 ƥ��ɹ�֮��replace��������POSITION_LOCATION�ַ����滻Ϊ��0��
		
		//pattern-replace
		std::unordered_map<std::string, std::string> replaceMap = {
			{"POSITION_LOCATION", std::to_string(LOCATION_MAP.at("position"))},
			{"NORMAL_LOCATION", std::to_string(LOCATION_MAP.at("normal"))},
			{"UV_LOCATION", std::to_string(LOCATION_MAP.at("uv"))},
			{"COLOR_LOCATION", std::to_string(LOCATION_MAP.at("color"))},
			{"SKINNING_INDICES_LOCATION", std::to_string(LOCATION_MAP.at("skinIndex"))},
			{"SKINNING_WEIGHTS_LOCATION", std::to_string(LOCATION_MAP.at("skinWeight"))},
			{"TANGENT_LOCATION", std::to_string(LOCATION_MAP.at("tangent"))},
			{"BITANGENT_B_LOCATION", std::to_string(LOCATION_MAP.at("bitangent"))},
		};

		for (const auto& iter : replaceMap) {
			//iter.first = ռλ���ַ���
			//iter.second = location���ֵ��ַ���

			//ʹ��c++��������ʽ�����滻��ֱ��ʹ��ռλ�����ַ�����ʼ����regex
			std::regex pattern(iter.first);

			//ɨ������shader�ַ�����ֻҪ���ַ���pattern���ַ�������Ҫ�滻Ϊiter.second
			shader = std::regex_replace(shader, pattern, iter.second);
		}
	}

	void DriverProgram::replaceLightNumbers(std::string& shader, const Parameters::Ptr& parameters) noexcept {
		//pattern-replace
		std::unordered_map<std::string, std::string> replaceMap = {
			{"NUM_DIR_LIGHTS", std::to_string(parameters->mDirectionalLightCount)},
			{"NUM_DIR_LIGHT_SHADOWS", std::to_string(parameters->mNumDirectionalLightShadows)},
		};

		for (const auto& iter : replaceMap) {
			std::regex pattern(iter.first);
			shader = std::regex_replace(shader, pattern, iter.second);
		}
	}

	std::string DriverProgram::getExtensionString() noexcept {
		std::string extensionString = "";
		extensionString.append("#extension GL_ARB_separate_shader_objects : enable\n");

		return extensionString;
	}

	void DriverProgram::uploadUniforms(UniformHandleMap& uniformMap, const DriverTextures::Ptr& textures) {
		mUniforms->upload(uniformMap, textures);
	}

//--------driver programs----------------------------
	DriverPrograms::DriverPrograms() noexcept {}

	DriverPrograms::~DriverPrograms() noexcept {}

	DriverProgram::Ptr DriverPrograms::acquireProgram(const DriverProgram::Parameters::Ptr& parameters, HashType cacheKey) noexcept {
		//��Ȼ�ڵ�ǰMaterial��Ӧ��DriverMaterial���У���û���ҵ�����ʹ�ù��ķ��ϱ�Paramters��DriverProgram
		//���Ǵ�ȫ�ֽǶȣ�����Material��������ʹ�ù����ϱ�Parameters��DriverProgram
		auto iter = mPrograms.find(cacheKey);

		if (iter != mPrograms.end()) {
			return iter->second;
		}

		auto program = DriverProgram::create(parameters);
		program->mCacheKey = cacheKey;
		mPrograms.insert(std::make_pair(cacheKey, program));
		//һ�����ñ����������ⲿ�϶���һ��renderItem��Ҫ���ñ�program
		//�����ж��material������ʹ�ù���ǰ���DriverProgram,���赱ǰ����������壬������ͬ��materials�����Ҽ���
		//������materials��ʹ���˱�DriverProgram����ôRefCount����2
		program->mRefCount++;

		return program;
	}

	//�����������ã���ζ�����ĳ��renderItem�ͷ��˶Ա�Driverprogram��ʹ��
	void DriverPrograms::release(const DriverProgram::Ptr& program) noexcept {
		if (--program->mRefCount == 0) {
			mPrograms.erase(program->mCacheKey);
		}
	}

	DriverProgram::Parameters::Ptr DriverPrograms::getParameters(
		const Material::Ptr& material,
		const Object3D::Ptr& object,
		const DriverLights::Ptr& lights,
		const DriverShadowMap::Ptr& shadowMap
	) noexcept {
		auto renderObject = std::static_pointer_cast<RenderableObject>(object);
		auto geometry = renderObject->getGeometry();

		//�½�һ��parameters
		auto parameters = DriverProgram::Parameters::create();

		auto shaderID = material->getType();
		auto shaderIter = ShaderLib.find(shaderID);

		if (shaderIter == ShaderLib.end()) {
			return nullptr;
		}

		parameters->mShaderID = material->getType();
		//shaderIter->second �� shader struct object
		parameters->mVertex = shaderIter->second.mVertex;
		parameters->mFragment = shaderIter->second.mFragment;

		parameters->mShadowMapEnabled = shadowMap->mEnabled;
		parameters->mDirectionalLightCount = lights->mState.mDirectionalCount;
		parameters->mNumDirectionalLightShadows = lights->mState.mNumDirectionalShadows;

		if (geometry->hasAttribute("normal")) {
			parameters->mHasNormal = true;
		}

		if (geometry->hasAttribute("uv")) {
			parameters->mHasUV = true;
		}

		if (geometry->hasAttribute("color")) {
			parameters->mHasColor = true;
		}

		if (material->mDiffuseMap != nullptr) {
			parameters->mHasDiffuseMap = true;
		}

		if (material->mEnvMap != nullptr) {
			parameters->mHasEnvCubeMap = true;
		}

		if (material->mSpecularMap != nullptr) {
			parameters->mHasSpecularMap = true;
		}

		if (material->mNormalMap != nullptr) {
			parameters->mUseNormalMap = true;
		}
		
		if (object->mIsRenderableObject) {
			auto renderableObject = std::static_pointer_cast<RenderableObject>(object);
			auto geometry = renderableObject->getGeometry();
			parameters->mUseTangent = geometry->hasAttribute("tangent");
		}

		if (material->mIsDepthMaterial) {
			auto depthMaterial = std::static_pointer_cast<DepthMaterial>(material);
			parameters->mDepthPacking = depthMaterial->mPacking;
		}

		if (object->mIsSkinnedMesh) {
			auto skinnedMesh = std::static_pointer_cast<SkinnedMesh>(object);
			parameters->mSkinning = true;
			parameters->mMaxBones = skinnedMesh->mSkeleton->mBones.size();
		}

		return parameters;
	}

	//���ݴ���Material���͵Ĳ�ͬ������������UniformHandleMap
	UniformHandleMap DriverPrograms::getUniforms(const Material::Ptr& material) noexcept {
		UniformHandleMap uniforms{};

		auto shaderID = material->getType();
		auto shaderIter = ShaderLib.find(shaderID);

		if (shaderIter != ShaderLib.end()) {
			uniforms = shaderIter->second.mUniformMap;
		}

		return uniforms;
	}

	//��parameters�����ַ�����Ȼ����й�ϣ���㣬�õ����յĹ�ϣ���
	HashType DriverPrograms::getProgramCacheKey(const DriverProgram::Parameters::Ptr& parameters) noexcept {
		std::hash<std::string> hasher;

		std::string keyString;

		keyString.append(parameters->mShaderID);
		keyString.append(parameters->mVertex);
		keyString.append(parameters->mFragment);
		keyString.append(std::to_string(parameters->mHasNormal));
		keyString.append(std::to_string(parameters->mHasUV));
		keyString.append(std::to_string(parameters->mHasColor));
		keyString.append(std::to_string(parameters->mHasDiffuseMap));
		keyString.append(std::to_string(parameters->mHasEnvCubeMap));
		keyString.append(std::to_string(parameters->mHasSpecularMap));
		keyString.append(std::to_string(parameters->mDirectionalLightCount));
		keyString.append(std::to_string(parameters->mNumDirectionalLightShadows));
		keyString.append(std::to_string(parameters->mSkinning));
		keyString.append(std::to_string(parameters->mMaxBones));
		keyString.append(std::to_string(parameters->mUseNormalMap));
		keyString.append(std::to_string(parameters->mUseTangent));
		keyString.append(std::to_string(parameters->mDepthPacking));

		return hasher(keyString);
	}
}