#include <Windows.h>
#include<iostream>
#include "application/application.h"
#include "gpu/gpu.h"

#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup" )


void render() {
	sgl->clear();

	for (uint32_t i = 0; i < app->getWidth(); ++i) {
		for (uint32_t j = 0; j < app->getHeight(); ++j) {
			uint32_t v = std::rand() % 255;
			RGBA color(v, v, v, v);
			sgl->drawPoint(i, j, color);
		}
	}
}

int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,		//本应用程序实例句柄，唯一指代当前程序
	_In_opt_ HINSTANCE hPrevInstance,	//本程序前一个实例，一般是null
	_In_ LPWSTR    lpCmdLine,		//应用程序运行参数
	_In_ int       nCmdShow)		//窗口如何显示（最大化、最小化、隐藏），不需理会
{
	if (!app->initApplication(hInstance, 800, 600)) {
		return -1;
	}

	//将bmp指向的内存配置到sgl当中 
	sgl->initSurface(app->getWidth(), app->getHeight(), app->getCanvas());

	bool alive = true;
	while (alive) {
		alive = app->peekMessage();
		render();
		app->show();
	}

	return 0;
}
