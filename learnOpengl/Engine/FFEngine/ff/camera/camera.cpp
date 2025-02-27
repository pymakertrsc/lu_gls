#include "camera.h"

namespace ff {

	Camera::Camera() noexcept {
		mIsCamera = true;
	}

	Camera::~Camera() noexcept {}

	glm::mat4 Camera::updateWorldMatrix(bool updateParent, bool updateChildren) noexcept {
		Object3D::updateWorldMatrix(updateParent, updateChildren);
		mWorldMatrixInverse = glm::inverse(mWorldMatrix);

		return mWorldMatrix;
	}
}