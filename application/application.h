#pragma once
#include "../global/base.h"
#include<Windows.h>
#include "camera.h"

#define APP Application::getInstance()

class Application {
public:
	static Application* getInstance();
	Application();
	~Application();

	//注册窗体类，创建一个窗体，显示
	bool initApplication(HINSTANCE hInstance, const uint32_t& width = 800, const uint32_t& height = 600);

	//托管了wndProc捕获的消息，并且进行处理
	void handleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//每一帧/每一次循环，都会调用，捕获以及分发窗体消息
	bool peekMessage();

	void show();

	uint32_t getWidth() const { return mWidth; }
	uint32_t getHeight() const { return mHeight; }
	void* getCanvas() const { return mCanvasBuffer; }

	void setCamera(Camera* camera);

private:
	BOOL createWindow(HINSTANCE hInstance);
	ATOM registerWindowClass(HINSTANCE hInstance);

private:
	static Application* mInstance;

	Camera* mCamera{ nullptr };

	//为true表示当前窗体仍然在继续显示，程序一直在跑
	//为false表示窗体已经被命令关闭，程序需要退出
	bool		mAlive{ true };

	HINSTANCE	mWindowInst;
	WCHAR		mWindowClassName[100] = L"AppWindow";
	HWND		mHwnd;

	int			mWidth = 800;
	int			mHeight = 600;

	HDC			mhDC;//当前窗口主dc
	HDC			mCanvasDC;//创建的于mhDC相兼容的绘图用的dc
	HBITMAP		mhBmp;//mCanvasDC内部生成的bitmap
	void*		mCanvasBuffer{ nullptr };//mhBmp对应的内存起始位置指针
};
