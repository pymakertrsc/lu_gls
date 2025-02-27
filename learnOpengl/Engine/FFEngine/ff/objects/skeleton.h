#pragma once
#include "../global/base.h"
#include "bone.h"
#include "../render/shaders/uniformsLib.h"

namespace ff {

	//��Ӧ��һ��SkinnedMesh�յ�������Ӱ�쵽���Ĺ���
	class Skeleton {
	public:
		using Ptr = std::shared_ptr<Skeleton>;
		static Ptr create(const std::vector<Bone::Ptr>& bones, const std::vector<glm::mat4>& offsetMatrices) {
			return std::make_shared<Skeleton>(bones, offsetMatrices);
		}

		Skeleton(const std::vector<Bone::Ptr>& bones, const std::vector<glm::mat4>& offsetMatrices) noexcept;

		~Skeleton() noexcept;

		//һ��bone�����֣��������Ӧ��aiNode������
		Bone::Ptr getBoneByName(const std::string& name) noexcept;

		void update() noexcept;

	public:
		std::vector<Bone::Ptr> mBones{};
		std::vector<glm::mat4> mOffsetMatrices{};

		UniformHandleMap mUniforms{};
	};
}