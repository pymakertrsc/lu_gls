#pragma once

#include "../global/base.h"

namespace ff {

	class Plane {
	public:
		using Ptr = std::shared_ptr<Plane>;
		static Ptr create(const glm::vec3& normal, float constant) {
			return std::make_shared<Plane>(normal, constant);
		}

		Plane(const glm::vec3& normal, float constant) noexcept {
			mNormal = glm::normalize(normal);
			mConstant = constant;
		}

		~Plane() noexcept {}

		//��ȡһ���㵽ƽ��ľ��루�뷨��ͨ���߾���Ϊ�����뷨�߷����߾���Ϊ����
		float distanceToPoint(const glm::vec3& point) noexcept {
			return glm::dot(mNormal, point) + mConstant;
		}

		void setComponents(float x, float y, float z, float w) noexcept {
			mNormal.x = x;
			mNormal.y = y;
			mNormal.z = z;
			
			auto length = glm::length(mNormal);
			mNormal /= length;
			mConstant = w/length;
		}

	public:
		glm::vec3	mNormal = glm::vec3(0.0f);
		float		mConstant{ 0.0f };
	};
}