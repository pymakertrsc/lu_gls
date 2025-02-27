#pragma once
#include "../global/base.h"
#include "../global/constant.h"
#include "../camera/camera.h"

namespace ff {

	//���񣺸��ݼ�����꣬����camera�ľ���
	//���ࣺgameCameraControl wasd ����Ҽ� trackBallControl cad/cae�������
	class CameraControl {
	public:
		using Ptr = std::shared_ptr<CameraControl>;

		CameraControl(const Camera::Ptr& camera) noexcept;

		~CameraControl() noexcept;

		//��������������bitλ���ϣ�һ��108��
		virtual void onKeyboard(const KeyBoardState& action) noexcept;

		virtual void onMouseAction(const MouseAction& action) noexcept;

		virtual void onMouseMove(double xpos, double ypos) noexcept;

	protected:
		Camera::Ptr	mCamera{ nullptr };//�������  ͸�����
	};
}