#pragma once
#include "../global/base.h"
#include "../core/object3D.h"

namespace ff {

	//��ʹ��Assimp��ȡģ�͵�ʱ��ÿһ��aiNode�����γ�һ��Bone
	//
	class Bone : public Object3D {
	public:
		using Ptr = std::shared_ptr<Bone>;
		static Ptr create() {
			return std::make_shared<Bone>();
		}

		Bone() noexcept;

		~Bone() noexcept;

		glm::mat4 updateWorldMatrix(bool updateParent = false, bool updateChildren = false) noexcept override;

	public:
		//��Ӧ�ŵ�Node��mTransform
		glm::mat4 mNodeMatrix = glm::mat4(1.0f);
	};
}