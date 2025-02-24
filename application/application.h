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

	bool initApplication(HINSTANCE hInstance, const uint32_t& width = 800, const uint32_t& height = 600);

	void handleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

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


	bool		mAlive{ true };

	HINSTANCE	mWindowInst;
	WCHAR		mWindowClassName[100] = L"AppWindow";
	HWND		mHwnd;

	int			mWidth = 800;
	int			mHeight = 600;

	HDC			mhDC;
	HDC			mCanvasDC;
	HBITMAP		mhBmp;
	void*		mCanvasBuffer{ nullptr };
};
