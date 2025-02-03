#pragma once
#include "../global/base.h"
#include "frameBuffer.h"
#include "../application/application.h"
#include "../application/image.h"

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

	void drawTriangle(const Point& p1, const Point& p2, const Point& p3);

	void drawImage(const Image* image);

	void drawImageWidthAlpha(const Image* image, const uint32_t& alpha);

	void setBlending(bool enable);

private:
	static GPU* mInstance;

	bool mEnableBlending{ false };

	FrameBuffer* mFrameBuffer{ nullptr };
};