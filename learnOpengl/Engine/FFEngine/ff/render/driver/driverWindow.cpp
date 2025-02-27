#include "driverWindow.h"
#include "../renderer.h"

namespace ff {

	DriverWindow::DriverWindow(Renderer* renderer, const int& width, const int& height) noexcept {
		mWidth = width;
		mHeight = height;
		mRenderer = renderer;

		glfwInit();

		//��ʼ����������Ϣ
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

		//��������
		mWindow = glfwCreateWindow(mWidth, mHeight, "FunForestEngine window", nullptr, nullptr);
		if (!mWindow) {
			std::cerr << "Error: failed to create window" << std::endl;
			glfwTerminate();
			exit(0);
		}

		glfwMakeContextCurrent(mWindow);

		//��Ϊgladһ��ʼ�ĺ���ָ�붼������״̬����Ҫͨ��glfw��ϵͳ��ص����ԣ�������
		//os�Ĳ�ͬ����Ϊ��Щ����ָ�븳ֵ��װ�أ�
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			exit(0);
		}

		//����ǰ��DriverWindow�����Ķ����thisָ�룬��Ϊ��һ��userData��������GLFWWindow��mWindow��
		//GLFWWindow Class��ʵ���溬��һ��void*��ָ�룬��Ϊ�û����Զ��������(������EventBase���е�UserData��
		glfwSetWindowUserPointer(mWindow, this);

		//���ø���ص�����
		glfwSetFramebufferSizeCallback(mWindow, frameSizeCallback);

		glfwSetMouseButtonCallback(mWindow, mouseActionCallback);

		glfwSetCursorPosCallback(mWindow, mouseMoveCallback);

		//close the v-sync for screen 
		// ��ֱͬ���Ĺرգ���ʾ����һ����ˢ��Ƶ�ʣ�����ʱ���Ⱦ�ٶ�Ҫ���ܶ࣬����
		// ��ֱͬ����ʱ����Ⱦ�ٶȻ���ˢ��Ƶ��һ��
		//glfwSwapInterval(0);

	}

	DriverWindow::~DriverWindow() noexcept {
		glfwDestroyWindow(mWindow);
		glfwTerminate();
	}

	bool DriverWindow::processEvent() noexcept {

		//��������˳�����
		//glfwGetKey ��ȡ��ǰ�����ĳ��������״̬
		if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwWindowShouldClose(mWindow)) {
			glfwSetWindowShouldClose(mWindow, true);
			return false;
		}

		//��ÿһ֡������¼����м��е���Ӧ
		glfwPollEvents();

		//���д�������¼�

		bool needsNotify{ false };//���̵�ĳһ������ĳ����״ֻ̬Ҫ�����˸ı�,������Ϊtrue,����״̬���û�б仯,����Ҫ֪ͨ
		auto iter = KeyboardActionMap.begin();
		while (iter != KeyboardActionMap.end()) {
			//����ÿһ�����ǹ��ĵİ���,���ȼ����״̬(����.̧��)
			//����mKeyState�����:
			//���ǹ涨�İ������Ϊ:
			//WKey = 1;
			//SKey = 2;
			//AKey = 3;
			//DKey = 4;
			//KeyNONE = 31;
			//mKeyState����bitset,��һ��bit��λ����:00001001100
			//ÿһ��bitλ��������Ӧ�ļ��̰����İ���(1)̧��(0)

			//iter->first��ʾ��glfw�����ĳ������
			//iter->second��ʾ�����Ƕ�����������ı��
			if (glfwGetKey(mWindow, iter->first) == GLFW_PRESS)
			{
				//��Ϊ1,��ʾ֮ǰû�м�¼���µ�״̬
				if (!mKeyState[iter->second]) {
					//set��������ǰ���λ�õ�bit����Ϊ1
					mKeyState.set(iter->second);
					needsNotify = true;
				}
			}
			else if (glfwGetKey(mWindow, iter->first) == GLFW_RELEASE) {
				//�����ǰ��������ǰ��µ�״̬,�������״̬
				if (mKeyState[iter->second]) {
					//reset��������ǰ���λ�õ�bit����Ϊ0
					mKeyState.reset(iter->second);
					needsNotify = true;
				}
			}
			iter++;
		}

		if (mKeyboardActionCallback && needsNotify) {
			mKeyboardActionCallback(mKeyState);
		}

		return true;
	}

	void DriverWindow::swap() noexcept {
		glfwSwapBuffers(mWindow);
	}

	void DriverWindow::setFrameSizeCallBack(const FrameSizeCallback& callback) noexcept {
		mFrameSizeCallback = callback;
	}

	void DriverWindow::setMouseMoveCallBack(const MouseMoveCallback& callback) noexcept {
		mMouseMoveCallback = callback;
	}

	void DriverWindow::setMouseActionCallback(const MouseActionCallback& callback) noexcept {
		mMouseActionCallback = callback;
	}

	void DriverWindow::setKeyboardActionCallBack(const KeyboardActionCallback& callback) noexcept {
		mKeyboardActionCallback = callback;
	}

	void DriverWindow::frameSizeCallback(GLFWwindow* window, int width, int height) noexcept {
		//��������һ����̬�������޷��õ���DriverWindow��thisָ��
		//����Ӧ��DriverWindowָ�룬��glfwWindow�����û������û�������һ��void*
		//���Ƿǳ�֮ȷ�������void*ָ�룬һ����DriverWindow��ָ��
		auto self = static_cast<DriverWindow*>(glfwGetWindowUserPointer(window));
		self->mWidth = width;
		self->mHeight = height;

		if (self->mFrameSizeCallback) {
			self->mFrameSizeCallback(self, width, height);
		}

	}

	void DriverWindow::mouseMoveCallback(GLFWwindow* window, double xpos, double ypos) noexcept {
		//��������driverWindow��ָ��
		auto self = static_cast<DriverWindow*>(glfwGetWindowUserPointer(window));

		if (self->mMouseMoveCallback) {
			self->mMouseMoveCallback(xpos, ypos);
		}
	}

	void DriverWindow::mouseActionCallback(GLFWwindow* window, int button, int action, int mods) noexcept {
		//glfw���������ĸ����������˷�Ӧ(������)-button,�����������Ĳ���(���,̧��)-action

		//��������driverWindow��ָ��
		auto self = static_cast<DriverWindow*>(glfwGetWindowUserPointer(window));

		MouseAction mouseAction{ MouseAction::NONE };

		//�������򵥵ķ�ʽ:
		// 1 ����ĸ��������˷�Ӧ
		// 2 ��鰴�»���̧��
		// if(button == left){
		//	 if(action == down) {
		//	 }else if(action == up) {
		//	 }
		// }
		//
		auto iter = MouseActionMap.begin();
		while (iter != MouseActionMap.end()) {
			//iter->first �������ĸ�����������Ӧ
			//iter->secondӵ����������,��һ����down �ڶ�����up

			//�����ж����Ĳ���״̬,down up?
			if (action == GLFW_PRESS) {

				//��һ������down��?
				if (button == iter->first) {
					//��tuple����,�õ���0��Ԫ��
					mouseAction = std::get<DOWN>(iter->second);
					break;
				}
			}
			else if (action == GLFW_RELEASE) {
				if (button == iter->first) {
					//��tuple����,�õ���1��Ԫ��
					mouseAction = std::get<UP>(iter->second);
					break;
				}
			}

			iter++;
		}

		if (iter == MouseActionMap.end()) {
			return;
		}

		if (self->mMouseActionCallback) {
			self->mMouseActionCallback(mouseAction);
		}
	}
}