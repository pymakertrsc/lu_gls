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

			//�������������յ��Լ���ϵͳӰ�죬
			if (parent->mIsBone) {
				mWorldMatrix = parent->getWorldMatrix() * mWorldMatrix;
			}
		}
 
		//û�ж�����������Nodematrix
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