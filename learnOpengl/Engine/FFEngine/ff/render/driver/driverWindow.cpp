#include "driverWindow.h"
#include "../renderer.h"

namespace ff {

	DriverWindow::DriverWindow(Renderer* renderer, const int& width, const int& height) noexcept {
		mWidth = width;
		mHeight = height;
		mRenderer = renderer;

		glfwInit();

		//初始化上下文信息
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

		//创建窗体
		mWindow = glfwCreateWindow(mWidth, mHeight, "FunForestEngine window", nullptr, nullptr);
		if (!mWindow) {
			std::cerr << "Error: failed to create window" << std::endl;
			glfwTerminate();
			exit(0);
		}

		glfwMakeContextCurrent(mWindow);

		//因为glad一开始的函数指针都是悬空状态，需要通过glfw跟系统相关的特性，来根据
		//os的不同，来为这些函数指针赋值（装载）
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			exit(0);
		}

		//将当前的DriverWindow这个类的对象的this指针，作为了一个userData，传给了GLFWWindow（mWindow）
		//GLFWWindow Class其实里面含有一个void*的指针，作为用户可自定义的数据(类似于EventBase当中的UserData）
		glfwSetWindowUserPointer(mWindow, this);

		//设置各类回调函数
		glfwSetFramebufferSizeCallback(mWindow, frameSizeCallback);

		glfwSetMouseButtonCallback(mWindow, mouseActionCallback);

		glfwSetCursorPosCallback(mWindow, mouseMoveCallback);

		//close the v-sync for screen 
		// 垂直同步的关闭，显示器有一定的刷新频率，大概率比渲染速度要慢很多，开启
		// 垂直同步的时候，渲染速度会与刷新频率一致
		//glfwSwapInterval(0);

	}

	DriverWindow::~DriverWindow() noexcept {
		glfwDestroyWindow(mWindow);
		glfwTerminate();
	}

	bool DriverWindow::processEvent() noexcept {

		//处理程序退出问题
		//glfwGetKey 获取当前窗体的某个按键的状态
		if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwWindowShouldClose(mWindow)) {
			glfwSetWindowShouldClose(mWindow, true);
			return false;
		}

		//在每一帧都会对事件进行集中的响应
		glfwPollEvents();

		//集中处理键盘事件

		bool needsNotify{ false };//键盘的某一个或者某几个状态只要发生了改变,都会置为true,键盘状态如果没有变化,不需要通知
		auto iter = KeyboardActionMap.begin();
		while (iter != KeyboardActionMap.end()) {
			//对于每一个我们关心的按键,首先检查其状态(按下.抬起)
			//对于mKeyState的理解:
			//咱们规定的按键编号为:
			//WKey = 1;
			//SKey = 2;
			//AKey = 3;
			//DKey = 4;
			//KeyNONE = 31;
			//mKeyState属于bitset,即一堆bit单位集合:00001001100
			//每一个bit位代表了响应的键盘按键的按下(1)抬起(0)

			//iter->first表示了glfw里面的某个按键
			//iter->second表示了我们对于这个按键的编号
			if (glfwGetKey(mWindow, iter->first) == GLFW_PRESS)
			{
				//不为1,表示之前没有记录按下的状态
				if (!mKeyState[iter->second]) {
					//set函数将当前这个位置的bit设置为1
					mKeyState.set(iter->second);
					needsNotify = true;
				}
			}
			else if (glfwGetKey(mWindow, iter->first) == GLFW_RELEASE) {
				//如果当前这个按键是按下的状态,则更新其状态
				if (mKeyState[iter->second]) {
					//reset函数将当前这个位置的bit设置为0
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
		//本函数是一个静态函数，无法拿到本DriverWindow的this指针
		//将对应的DriverWindow指针，从glfwWindow里面拿回来，拿回来的是一个void*
		//我们非常之确定，这个void*指针，一定是DriverWindow的指针
		auto self = static_cast<DriverWindow*>(glfwGetWindowUserPointer(window));
		self->mWidth = width;
		self->mHeight = height;

		if (self->mFrameSizeCallback) {
			self->mFrameSizeCallback(self, width, height);
		}

	}

	void DriverWindow::mouseMoveCallback(GLFWwindow* window, double xpos, double ypos) noexcept {
		//解析出来driverWindow的指针
		auto self = static_cast<DriverWindow*>(glfwGetWindowUserPointer(window));

		if (self->mMouseMoveCallback) {
			self->mMouseMoveCallback(xpos, ypos);
		}
	}

	void DriverWindow::mouseActionCallback(GLFWwindow* window, int button, int action, int mods) noexcept {
		//glfw给到我们哪个按键发生了反应(左右中)-button,发生了怎样的操作(点击,抬起)-action

		//解析出来driverWindow的指针
		auto self = static_cast<DriverWindow*>(glfwGetWindowUserPointer(window));

		MouseAction mouseAction{ MouseAction::NONE };

		//如果用最简单的方式:
		// 1 检查哪个按键有了反应
		// 2 检查按下或者抬起
		// if(button == left){
		//	 if(action == down) {
		//	 }else if(action == up) {
		//	 }
		// }
		//
		auto iter = MouseActionMap.begin();
		while (iter != MouseActionMap.end()) {
			//iter->first 代表了哪个按键发生反应
			//iter->second拥有两个数据,第一个是down 第二个是up

			//首先判断鼠标的操作状态,down up?
			if (action == GLFW_PRESS) {

				//哪一个按键down了?
				if (button == iter->first) {
					//从tuple里面,拿到第0个元素
					mouseAction = std::get<DOWN>(iter->second);
					break;
				}
			}
			else if (action == GLFW_RELEASE) {
				if (button == iter->first) {
					//从tuple里面,拿到第1个元素
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