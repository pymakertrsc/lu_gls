#pragma once
#include "../global/base.h"
#include "frameBuffer.h"
#include "../application/application.h"
#include "../application/image.h"
#include "../math/math.h"

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

	void setTexture(Image* image);

private:
	RGBA sampleNearest(const math::vec2f& uv);

private:
	static GPU* mInstance;

	bool mEnableBlending{ false };

	FrameBuffer* mFrameBuffer{ nullptr };

	Image* mImage{ nullptr };//采样对象，没有对象就直接插值
};