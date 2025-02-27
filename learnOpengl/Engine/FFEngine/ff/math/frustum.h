#pragma once 
#include "../global/base.h"
#include "plane.h"
#include "sphere.h"
#include "../objects/renderableObject.h"

namespace ff {

	class Frustum {
	public:
		using Ptr = std::shared_ptr<Frustum>;
		static Ptr create() {
			return std::make_shared<Frustum>();
		}

		Frustum() noexcept {
			mToolSphere = Sphere::create(glm::vec3(0.0f), 0.0f);
			for (uint32_t i = 0; i < 6; ++i) {
				mPlanes.push_back(Plane::create(glm::vec3(0.0f), 0.0f));
			}
		}

		//https://zhuanlan.zhihu.com/p/491340245
		void setFromProjectionMatrix(const glm::mat4& matrix) {
			auto m = glm::value_ptr(matrix);

			mPlanes[0]->setComponents(m[3] - m[0], m[7] - m[4], m[11] - m[8], m[15] - m[12]);
			mPlanes[1]->setComponents(m[3] + m[0], m[7] + m[4], m[11] + m[8], m[15] + m[12]);
			mPlanes[2]->setComponents(m[3] + m[1], m[7] + m[5], m[11] + m[9], m[15] + m[13]);
			mPlanes[3]->setComponents(m[3] - m[1], m[7] - m[5], m[11] - m[9], m[15] - m[13]);
			mPlanes[4]->setComponents(m[3] - m[2], m[7] - m[6], m[11] - m[10], m[15] - m[14]);
			mPlanes[5]->setComponents(m[3] + m[2], m[7] + m[6], m[11] + m[10], m[15] + m[14]);
		}

		bool intersectObject(const RenderableObject::Ptr& object) noexcept {
			auto geometry = object->getGeometry();

			if (geometry->getBoundingSphere() == nullptr) {
				geometry->computeBoundingSphere();
			}

			//�ܶ�ͬѧ��Ϊ����ֱ����toolSphere = geometry->boundingSphere
			//�·�����Ҫ�԰�Χ�����worldMatrix�ı任
			//��Ϊ�����õ�������ָ�룬�������ֱ��ʩ�ӱ任���ͻᵼ��geometry����İ�Χ�򱻲�ͣ�ı任�ۼ�
			mToolSphere->copy(geometry->getBoundingSphere());
			mToolSphere->applyMatrix4(object->getWorldMatrix());

			return intersectSphere(mToolSphere);
		}

		bool intersectSphere(const Sphere::Ptr& sphere) noexcept {
			auto center = sphere->mCenter;
			auto radius = sphere->mRadius;

			for (uint32_t i = 0; i < 6; ++i) {
				//1 �����Χ������ĵ���ǰƽ��ľ���
				auto distance = mPlanes[i]->distanceToPoint(center);

				//2 ���������ƽ������棬��ôdistanceһ�����������·�if����ȥ
				// ���������ƽ��ķ��棬��ôdistanceһ���Ǹ���������������ľ��������radius
				// ��-distance > radius �� distance < -radius ���������豾ƽ���ཻ��λ��ƽ�淴�棬��������Ҫ������
				if (distance < -radius) {
					return false;
				}
			}

			//�Ӿ������ͨ�������Ի���
			return true;
		}

	private:
		std::vector<Plane::Ptr> mPlanes{};
		Sphere::Ptr mToolSphere{ nullptr };
	};
}