#pragma once
#include "../global/base.h"
#include "bone.h"
#include "../render/shaders/uniformsLib.h"

namespace ff {

	//对应着一个SkinnedMesh收到的所有影响到它的骨骼
	class Skeleton {
	public:
		using Ptr = std::shared_ptr<Skeleton>;
		static Ptr create(const std::vector<Bone::Ptr>& bones, const std::vector<glm::mat4>& offsetMatrices) {
			return std::make_shared<Skeleton>(bones, offsetMatrices);
		}

		Skeleton(const std::vector<Bone::Ptr>& bones, const std::vector<glm::mat4>& offsetMatrices) noexcept;

		~Skeleton() noexcept;

		//一个bone的名字，就是其对应的aiNode的名字
		Bone::Ptr getBoneByName(const std::string& name) noexcept;

		void update() noexcept;

	public:
		std::vector<Bone::Ptr> mBones{};
		std::vector<glm::mat4> mOffsetMatrices{};

		UniformHandleMap mUniforms{};
	};
}