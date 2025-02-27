#include "gameCameraControl.h"

namespace ff {

	GameCameraControl::GameCameraControl(const Camera::Ptr& camera) noexcept :CameraControl(camera) {}

	GameCameraControl::~GameCameraControl() noexcept {}

	void GameCameraControl::setSpeed(float speed) noexcept {
		mSpeed = speed;
	}

	void GameCameraControl::setSensitivity(float s) noexcept {
		mSensitivity = s;
	}

	void GameCameraControl::onKeyboard(const KeyBoardState& action) noexcept {
		if (action[WKey]) {
			mMoveState[MoveFront] = true;
		}
		else {
			mMoveState[MoveFront] = false;
		}

		if (action[AKey]) {
			mMoveState[MoveLeft] = true;
		}
		else {
			mMoveState[MoveLeft] = false;
		}

		if (action[SKey]) {
			mMoveState[MoveBack] = true;
		}
		else {
			mMoveState[MoveBack] = false;
		}

		if (action[DKey]) {
			mMoveState[MoveRight] = true;
		}
		else {
			mMoveState[MoveRight] = false;
		}
	}

	void GameCameraControl::onMouseAction(const MouseAction& action) noexcept {
		if (action == MouseAction::RightDown) {
			mRotationState = true;
		}
		else if (action == MouseAction::RightUp) {
			mRotationState = false;
			//用来标定，是否第一次按下鼠标
			mMouseStateReset = true;
		}
	}

	void GameCameraControl::onMouseMove(double xpos, double ypos) noexcept {
		//如果右键没有按下，那么不处理旋转
		if (!mRotationState) {
			return;
		}

		if (mMouseStateReset) {
			mMouseStateReset = false;
		}
		else {

			float deltaX = xpos - mCurrentMousePosition.x;
			float deltaY = mCurrentMousePosition.y - ypos;

			float angleX = deltaX * mSensitivity;
			float angleY = deltaY * mSensitivity;

			mPitchAngle += angleY;
			mYawAngle += angleX;

			//不能仰面翻过去，也不能前滚翻
			if (mPitchAngle >= 90.0f) {
				mPitchAngle = 89.0f;
			}

			if (mPitchAngle <= -90.0f) {
				mPitchAngle = -89.0f;
			}

			mFront.y = sin(glm::radians(mPitchAngle));
			mFront.x = cos(glm::radians(mYawAngle)) * cos(glm::radians(mPitchAngle));
			mFront.z = sin(glm::radians(mYawAngle)) * cos(glm::radians(mPitchAngle));

			//一定要注意传进去的是看向哪个点，而不是直接把方向传进去
			mCamera->lookAt(mFront + mCamera->getPosition(), glm::vec3(0.0, 1.0, 0.0));
		}

		mCurrentMousePosition.x = xpos;
		mCurrentMousePosition.y = ypos;
	}

	//在onMouseMove的时候，已经计算完毕了camera的朝向， 即旋转矩阵，但是还没有更新位置
	void GameCameraControl::update() noexcept {
		//当前camera这个object的模型坐标系，负z方向
		glm::vec3 front = mCamera->getLocalDirection();

		//camera的右边( 模型坐标系)
		glm::vec3 right = mCamera->getRight();

		glm::vec3 position = mCamera->getPosition();

		glm::vec3 direction = glm::vec3(0.0f);
		bool needsUpdate = false;

		if (mMoveState[MoveFront]) {
			direction += front;
			needsUpdate = true;
		}

		if (mMoveState[MoveBack]) {
			direction += -front;
			needsUpdate = true;
		}

		if (mMoveState[MoveLeft]) {
			direction += -right;
			needsUpdate = true;
		}

		if (mMoveState[MoveRight]) {
			direction += right;
			needsUpdate = true;
		}

		if (needsUpdate && (direction.x != 0.0 || direction.y != 0.0 || direction.z != 0.0)) {
			//去除方向向量的长度影响
			direction = glm::normalize(direction);
			position += direction * mSpeed;

			mCamera->setPosition(position);
		}
	}
}