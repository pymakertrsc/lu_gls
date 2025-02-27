#pragma once

#include "../global/base.h"

namespace ff {

	class Sphere {
	public:
		//����ָ���װ
		using Ptr = std::shared_ptr<Sphere>;
		static Ptr create(const glm::vec3& center, float radius) {
			return std::make_shared<Sphere>(center, radius);
		}

		Sphere(const glm::vec3& center, float radius) noexcept {
			mCenter = center;
			mRadius = radius;
		}

		~Sphere() noexcept {}

		//Ӧ���ڸ����������matrix�任��ʱ��
		void applyMatrix4(const glm::mat4 matrix) noexcept {
			mCenter = glm::vec3(matrix * glm::vec4(mCenter, 1.0));

			//���ڰ뾶��ֻ���յ�scale����Ӱ�죬����ֻ��Ҫ��������Scale���������ĸ�
			float scaleX = glm::length(glm::vec3(matrix[0]));
			float scaleY = glm::length(glm::vec3(matrix[1]));
			float scaleZ = glm::length(glm::vec3(matrix[2]));

			float maxScale = std::max(std::max(scaleX, scaleY), scaleZ);
			mRadius *= maxScale;
		}

		void copy(const Sphere::Ptr& other) {
			//������
			//Sphere::Ptr s1 = Sphere::create(xxx);
			//Sphere::Ptr s2 = s1;
			//s2->mRadius = 10.0f;
			//�������Ļ����ͻᵼ��s1ָ����ڴ棬Ҳ�ᷢ���仯

			//Ӧ�ã�
			//Sphere::Ptr s2 = Sphere::create(xxx);
			//s2->copy(s1);
			mCenter = other->mCenter;
			mRadius = other->mRadius;
		}

	public:
		glm::vec3	mCenter = glm::vec3(0.0f);
		float		mRadius{ 0.0f };
	};
}