#include "cameraControl.h"

namespace ff {

	CameraControl::CameraControl(const Camera::Ptr& camera) noexcept {
		mCamera = camera;
	}

	CameraControl::~CameraControl() noexcept {}

	void CameraControl::onKeyboard(const KeyBoardState& action) noexcept {}

	void CameraControl::onMouseAction(const MouseAction& action) noexcept {}

	void CameraControl::onMouseMove(double xpos, double ypos) noexcept {}
}