#pragma once
#include "../global/base.h"
#include "../global/constant.h"
#include "../camera/camera.h"

namespace ff {

	//任务：根据键盘鼠标，更改camera的矩阵
	//种类：gameCameraControl wasd 鼠标右键 trackBallControl cad/cae软件当中
	class CameraControl {
	public:
		using Ptr = std::shared_ptr<CameraControl>;

		CameraControl(const Camera::Ptr& camera) noexcept;

		~CameraControl() noexcept;

		//本质上来讲就是bit位集合，一共108键
		virtual void onKeyboard(const KeyBoardState& action) noexcept;

		virtual void onMouseAction(const MouseAction& action) noexcept;

		virtual void onMouseMove(double xpos, double ypos) noexcept;

	protected:
		Camera::Ptr	mCamera{ nullptr };//正交相机  透视相机
	};
}