#pragma once

#include "../../global/base.h"
#include "../../global/constant.h"

namespace ff {

	class Renderer;
	//ʹ�õĿ���glfw
	class DriverWindow {
	public:

		//���巢��size�ı䶯
		//����䶯�Ƚ����⣬������������е�callback������ע������renderer���棬��renderer.cpp
		//���У���һ��onFrameSizeCallback��ע�ᵽ��DriverWindow�����¼�������ʱ��
		//glfw�����ȵ���frameSizeCallback,�����У�������onFrameSizeCallback
		//��onFrameSizeCallback���У�������renderer->setSize(xxx)
		//setSize����,��renderer������״̬����,����������callback����
		using FrameSizeCallback = std::function<void(DriverWindow* pDriverWindow, int width, int height)>;

		//����ƶ��ص�
		using MouseMoveCallback = std::function<void(double xpos, double ypos)>;

		//������Ӧ�ص� 
		using KeyboardActionCallback = std::function<void(KeyBoardState)>;

		//����¼���Ӧ
		using MouseActionCallback = std::function<void(MouseAction)>;

		//����ָ�����
		using Ptr = std::shared_ptr<DriverWindow>;
		static Ptr create(Renderer* renderer, const int& width, const int& height) { 
			return std::make_shared<DriverWindow>(renderer, width, height);
		}

		DriverWindow(Renderer* renderer, const int &width, const int &height) noexcept;

		~DriverWindow() noexcept;

		auto getWindow() const  noexcept { return mWindow; }

		//ÿһ֡��Ⱦǰ���������processEvent�����¼�������Ӧ
		bool processEvent() noexcept;

		//˫����ĸ���
		void swap() noexcept;
		
		//���ø����¼��Ļص�����
		void setFrameSizeCallBack(const FrameSizeCallback& callback) noexcept;

		void setMouseMoveCallBack(const MouseMoveCallback& callback) noexcept;

		void setMouseActionCallback(const MouseActionCallback& callback) noexcept;

		void setKeyboardActionCallBack(const KeyboardActionCallback& callback) noexcept;

	private:
		//glfw����¼������Ȼص��·��������������Ѿ�����windowע��Ľӿڣ���ô���·������л�������ص�����
		static void frameSizeCallback(GLFWwindow* window, int width, int height) noexcept;

		static void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos) noexcept;

		static void mouseActionCallback(GLFWwindow* window, int button, int action, int mods) noexcept;

	public:
		Renderer* mRenderer = nullptr;

	private:
		int mWidth{ 0 };
		int mHeight{ 0 };

		KeyBoardState mKeyState{};

		//�������������ػص�����
		FrameSizeCallback		mFrameSizeCallback{ nullptr };
		MouseMoveCallback		mMouseMoveCallback{ nullptr };
		KeyboardActionCallback	mKeyboardActionCallback{ nullptr };
		MouseActionCallback		mMouseActionCallback{ nullptr };

		GLFWwindow* mWindow{ NULL };
	};
}
