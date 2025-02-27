#pragma once
#include "../global/base.h"
#include "../core/object3D.h"

namespace ff {

	//worldMatrix:�������ģ������ϵ��ת������������ϵ
	//viewMatrix�����������������ϵ��ת����ģ������ϵ
	//�����Inverse Matrix��A��B��Ϊ�������ôA * B = I����λ��
	//Camera:һ������Ϊ��Ϸ���ӽǣ���Ϊ���յ����ӽ�
	class Camera:public Object3D {
	public:
		using Ptr = std::shared_ptr<Camera>;
	
		Camera() noexcept;

		~Camera() noexcept;

		glm::mat4 updateWorldMatrix(bool updateParent = false, bool updateChildren = false) noexcept override;

		glm::mat4 getWorldMatrixInverse() noexcept { return mWorldMatrixInverse; }

		glm::mat4 getProjectionMatrix() noexcept { return mProjectionMatrix; }

		virtual glm::mat4 updateProjectionMatrix() noexcept = 0;//���麯��,��Ҫ�����൱��ʵ��

	protected:
		glm::mat4 mWorldMatrixInverse = glm::mat4(1.0f);
		glm::mat4 mProjectionMatrix = glm::mat4(1.0f);

	};
}