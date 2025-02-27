#include "orthographicCamera.h"

namespace ff {

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float near, float far) noexcept {
		mIsOrthographicCamera = true;
		mLeft = left;
		mRight = right;
		mTop = top;
		mBottom = bottom;
		mNear = near;
		mFar = far;

		updateProjectionMatrix();
	}

	OrthographicCamera::~OrthographicCamera() noexcept {}

	glm::mat4 OrthographicCamera::updateProjectionMatrix() noexcept {
		mProjectionMatrix = glm::ortho(mLeft, mRight, mBottom, mTop, mNear, mFar);
		return mProjectionMatrix;
	}
}