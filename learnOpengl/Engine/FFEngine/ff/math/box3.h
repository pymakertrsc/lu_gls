#pragma once
#include "../global/base.h"
#include "../core/attribute.h"

namespace ff {

	class Box3 {
	public:
		//使用智能指针包装
		using Ptr = std::shared_ptr<Box3>;
		static Ptr create() {
			return std::make_shared<Box3>();
		}

		Box3() noexcept {}

		~Box3() noexcept {}

		//传入一个Mesh的PositionAttribute，给出其包围盒
		void setFromAttribute(const Attributef::Ptr& attribute) noexcept {
			for (uint32_t i = 0; i < attribute->getCount(); ++i) {
				float x = attribute->getX(i);
				float y = attribute->getY(i);
				float z = attribute->getZ(i);

				if (x < mMin.x) { mMin.x = x; }
				if (y < mMin.y) { mMin.y = y; }
				if (z < mMin.z) { mMin.z = z; }

				if (x > mMax.x) { mMax.x = x; }
				if (y > mMax.y) { mMax.y = y; }
				if (z > mMax.z) { mMax.z = z; }
			}
		}

		bool isEmpty() noexcept {
			return (mMax.x < mMin.x || mMax.y < mMin.y || mMax.z < mMin.z);
		}

		glm::vec3 getCenter() noexcept {
			if (isEmpty()) {
				return glm::vec3(0.0f);
			}

			//max与min处于立方体的对角线的两个顶点位置
			return (mMax + mMin) / 2.0f;
		}

	public:
		//min是包围盒xyz最小的那个点,max是包围盒xyz最大的那个点
		glm::vec3 mMin = glm::vec3(std::numeric_limits<float>::infinity());//用float最大值进行初始化
		glm::vec3 mMax = glm::vec3(-std::numeric_limits<float>::infinity());//用float最大值的负数进行初始化
	};
}