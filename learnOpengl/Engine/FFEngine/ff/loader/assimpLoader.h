#pragma once 
#include "../global/base.h"
#include "../objects/renderableObject.h"
#include "../objects/bone.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "../animation/animationClip.h"
#include "../animation/animationAction.h"
#include "../material/material.h"
#include "../textures/texture.h"


namespace ff {

	struct AssimpResult {
		using Ptr = std::shared_ptr<AssimpResult>;
		static Ptr create() {
			return std::make_shared<AssimpResult>();
		}

		std::vector<AnimationAction::Ptr> mActions;

		//最终解析完毕之后的RootNode
		Object3D::Ptr	mObject{ nullptr };
	};

	class AssimpLoader {
	public:
		AssimpLoader() noexcept {}

		~AssimpLoader() noexcept {}

		static AssimpResult::Ptr load(const std::string& path) noexcept;

	private:
		static void processNode(
			const aiNode* node,
			const aiScene* scene,
			Object3D::Ptr parentObject,
			const std::vector<Material::Ptr>& materials,
			const std::vector<Bone::Ptr>& bones);

		static void processSkeleton(
			const aiNode* node,
			const aiScene* scene,
			Bone::Ptr parentBone,
			std::vector<Bone::Ptr>& bones);

		static void processMaterial(
			const aiScene* scene,
			const std::string& rootPath,
			std::vector<Material::Ptr>& materials);

		static Texture::Ptr processTexture(
			const aiTextureType& type,
			const aiScene* scene,
			const aiMaterial* material,
			const std::string& rootPath);

		static Object3D::Ptr processMesh(
			const aiMesh* mesh,
			const aiScene* scene,
			const glm::mat4 localTransform,
			const std::vector<Material::Ptr>& material,
			const std::vector<Bone::Ptr>& bones);

		static std::vector<AnimationClip::Ptr> processAnimation(const aiScene* scene);

		static Bone::Ptr AssimpLoader::getBoneByName(const std::string& name, const std::vector<Bone::Ptr>& bones) noexcept;

		static void setVertexSkinData(
			const uint32_t& id, 
			const float& boneId,
			const float& skinWeight, 
			std::vector<float>& skinIndices,
			std::vector<float>& skinWeights);

		static glm::vec3 getGLMVec3(aiVector3D value) noexcept;

		static glm::quat getGLMQuat(aiQuaternion value) noexcept;

		static glm::mat4 getGLMMat4(aiMatrix4x4 value) noexcept;
	};
}