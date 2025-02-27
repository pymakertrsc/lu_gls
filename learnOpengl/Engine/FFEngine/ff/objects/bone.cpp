#include "bone.h"
#include "../log/debugLog.h"

namespace ff {

	Bone::Bone() noexcept {
		mIsBone = true;
	}

	Bone::~Bone() noexcept {}

	glm::mat4 Bone::updateWorldMatrix(bool updateParent, bool updateChildren) noexcept {
		if (!mParent.expired() && updateParent) {
			auto parent = mParent.lock();
			parent->updateWorldMatrix(true, false);
		}

		updateMatrix();

		mWorldMatrix = mLocalMatrix;

		if (!mParent.expired()) {
			auto parent = mParent.lock();

			//骨骼动画智能收到自己的系统影响，
			if (parent->mIsBone) {
				mWorldMatrix = parent->getWorldMatrix() * mWorldMatrix;
			}
		}
 
		//没有动画，就启用Nodematrix
		if (mLocalMatrix == glm::mat4(1.0)) {
			mWorldMatrix = mWorldMatrix * mNodeMatrix;
		}

		if (updateChildren) {
			for (auto& child : mChildren) {
				child->updateWorldMatrix(false, true);
			}
		}

		return mWorldMatrix;
		
	}
}