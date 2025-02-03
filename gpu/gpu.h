#pragma once
#include "../global/base.h"
#include "frameBuffer.h"
#include "../application/application.h"

#define sgl GPU::getInstance()


class GPU {
public:
	static GPU* getInstance();
	GPU();

	~GPU();

	
	void initSurface(const uint32_t& width, const uint32_t& height, void* buffer = nullptr);

	
	void clear();

	
	void drawPoint(const uint32_t& x, const uint32_t& y, const RGBA& color);

	void drawLine(const Point& p1, const Point& p2);

private:
	static GPU* mInstance;

	FrameBuffer* mFrameBuffer{ nullptr };
};