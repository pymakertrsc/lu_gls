#pragma once
#include "../global/base.h"
#include "frameBuffer.h"
#include "../application/application.h"

#define sgl GPU::getInstance()

/*
* class GPU：
* 模拟GPU的绘图行为以及算法等
*/
class GPU {
public:
	static GPU* getInstance();
	GPU();

	~GPU();

	//接受外界传入的bmp对应的内存指针以及窗体的宽/高
	void initSurface(const uint32_t& width, const uint32_t& height, void* buffer = nullptr);

	//清除画布内容
	void clear();

	//传入像素位置，绘制成某种颜色
	void drawPoint(const uint32_t& x, const uint32_t& y, const RGBA& color);

private:
	static GPU* mInstance;

	FrameBuffer* mFrameBuffer{ nullptr };
};