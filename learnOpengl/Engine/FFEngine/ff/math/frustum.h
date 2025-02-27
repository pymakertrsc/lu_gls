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

			//很多同学认为可以直接用toolSphere = geometry->boundingSphere
			//下方还需要对包围球进行worldMatrix的变换
			//因为我们用的是智能指针，所以如果直接施加变换，就会导致geometry里面的包围球被不停的变换累加
			mToolSphere->copy(geometry->getBoundingSphere());
			mToolSphere->applyMatrix4(object->getWorldMatrix());

			return intersectSphere(mToolSphere);
		}

		bool intersectSphere(const Sphere::Ptr& sphere) noexcept {
			auto center = sphere->mCenter;
			auto radius = sphere->mRadius;

			for (uint32_t i = 0; i < 6; ++i) {
				//1 计算包围球的球心到当前平面的距离
				auto distance = mPlanes[i]->distanceToPoint(center);

				//2 如果球心在平面的正面，那么distance一定是正数，下方if不进去
				// 如果球心在平面的反面，那么distance一定是负数，假设球与面的距离大于了radius
				// 即-distance > radius 则 distance < -radius 代表着球不予本平面相交且位于平面反面，则本物体需要被剪裁
				if (distance < -radius) {
					return false;
				}
			}

			//视景体剪裁通过，可以绘制
			return true;
		}

	private:
		std::vector<Plane::Ptr> mPlanes{};
		Sphere::Ptr mToolSphere{ nullptr };
	};
}