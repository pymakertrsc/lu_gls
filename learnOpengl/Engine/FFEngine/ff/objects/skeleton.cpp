#include "skeleton.h"

namespace ff {

	Skeleton::Skeleton(const std::vector<Bone::Ptr>& bones, const std::vector<glm::mat4>& offsetMatrices) noexcept {
		mBones = bones;
		mOffsetMatrices = offsetMatrices;

		UniformHandle boneMatricesUniform;
		boneMatricesUniform.mValue = std::vector<glm::mat4>();

		mUniforms["boneMatrices"] = boneMatricesUniform;
	}

	Skeleton::~Skeleton() noexcept {}

	Bone::Ptr Skeleton::getBoneByName(const std::string& name) noexcept {
		for (uint32_t i = 0; i < mBones.size(); ++i) {
			if (mBones[i]->mName == name) {
				return mBones[i];
			}
		}

		return nullptr;
	}

	//call after scene->updateWorldMatrix
	void Skeleton::update() noexcept {
		auto& boneMatricesUniform = mUniforms["boneMatrices"];
		boneMatricesUniform.mNeedsUpdate = true;
		clearPureArrayUniform(std::any_cast<std::vector<glm::mat4>>(&boneMatricesUniform.mValue));

		for (uint32_t i = 0; i < mBones.size(); ++i) {
			auto matrix = mBones[i]->getWorldMatrix() * mOffsetMatrices[i];
			pushPureArrayUniform(matrix, std::any_cast<std::vector<glm::mat4>>(&boneMatricesUniform.mValue));
		}
	}
}