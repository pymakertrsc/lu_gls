#pragma once 
#include "cameraControl.h"
#include "../global/constant.h"

namespace ff {

	class GameCameraControl :public CameraControl {
	public:
		static constexpr int MoveFront = 0;
		static constexpr int MoveBack = 1;
		static constexpr int MoveLeft = 2;
		static constexpr int MoveRight = 3;

		using Ptr = std::shared_ptr<GameCameraControl>;
		static Ptr create(const Camera::Ptr& camera) {
			return std::make_shared<GameCameraControl>(camera);
		}

		GameCameraControl(const Camera::Ptr& camera) noexcept;

		~GameCameraControl() noexcept;

		void setSpeed(float speed) noexcept;

		void setSensitivity(float s) noexcept;

		void onKeyboard(const KeyBoardState& action) noexcept override;

		void onMouseAction(const MouseAction& action) noexcept override;

		void onMouseMove(double xpos, double ypos) noexcept override;

		void update() noexcept;

	private:
		float mSpeed{ 0.1f };//�ƶ��ٶ�
		float mSensitivity{ 0.1f };//�����ȣ�����ƶ���ɶ��̶ȵ���ת

		glm::vec3	mFront = glm::vec3(0.0f);//��ǰ����ĳ���
		float		mPitchAngle{ 0.0f };
		float		mYawAngle{ -90.0f };

		//control parameters
		glm::vec2	mCurrentMousePosition = glm::vec2(0.0f);
		//������¼��ǰ�����ƶ��ķ��򣬿���ͬʱ���
		glm::bvec4	mMoveState = glm::bvec4(false);

		//�Ƿ�������ת,����Ҽ���ס֮�󣬿����϶�����������ת�ӽ�,̧��֮�󣬾Ͳ�����
		bool		mRotationState = false;
		bool		mMouseStateReset = true;
	};
}