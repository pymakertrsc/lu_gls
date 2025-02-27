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
			//�����궨���Ƿ��һ�ΰ������
			mMouseStateReset = true;
		}
	}

	void GameCameraControl::onMouseMove(double xpos, double ypos) noexcept {
		//����Ҽ�û�а��£���ô��������ת
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

			//�������淭��ȥ��Ҳ����ǰ����
			if (mPitchAngle >= 90.0f) {
				mPitchAngle = 89.0f;
			}

			if (mPitchAngle <= -90.0f) {
				mPitchAngle = -89.0f;
			}

			mFront.y = sin(glm::radians(mPitchAngle));
			mFront.x = cos(glm::radians(mYawAngle)) * cos(glm::radians(mPitchAngle));
			mFront.z = sin(glm::radians(mYawAngle)) * cos(glm::radians(mPitchAngle));

			//һ��Ҫע�⴫��ȥ���ǿ����ĸ��㣬������ֱ�Ӱѷ��򴫽�ȥ
			mCamera->lookAt(mFront + mCamera->getPosition(), glm::vec3(0.0, 1.0, 0.0));
		}

		mCurrentMousePosition.x = xpos;
		mCurrentMousePosition.y = ypos;
	}

	//��onMouseMove��ʱ���Ѿ����������camera�ĳ��� ����ת���󣬵��ǻ�û�и���λ��
	void GameCameraControl::update() noexcept {
		//��ǰcamera���object��ģ������ϵ����z����
		glm::vec3 front = mCamera->getLocalDirection();

		//camera���ұ�( ģ������ϵ)
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
			//ȥ�����������ĳ���Ӱ��
			direction = glm::normalize(direction);
			position += direction * mSpeed;

			mCamera->setPosition(position);
		}
	}
}