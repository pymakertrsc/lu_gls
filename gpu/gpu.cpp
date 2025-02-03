#include "gpu.h"
#include "raster.h"

GPU* GPU::mInstance = nullptr;
GPU* GPU::getInstance() {
	if (!mInstance) {
		mInstance = new GPU();
	}

	return mInstance;
}

GPU::GPU() {}

GPU::~GPU() {
	if (mFrameBuffer) {
		delete mFrameBuffer;
	}
}

void GPU::initSurface(const uint32_t& width, const uint32_t& height, void* buffer) {
	mFrameBuffer = new FrameBuffer(width, height, buffer);
}

void GPU::clear() {
	size_t pixelSize = mFrameBuffer->mWidth * mFrameBuffer->mHeight;
	std::fill_n(mFrameBuffer->mColorBuffer, pixelSize, RGBA(0, 0, 0, 0));
}

void GPU::drawPoint(const uint32_t& x, const uint32_t& y, const RGBA& color) {
	if (x >= mFrameBuffer->mWidth || y >= mFrameBuffer->mHeight) {
		return;
	}

	//从窗口左下角开始算起
	uint32_t pixelPos = y * mFrameBuffer->mWidth + x;


	RGBA result = color;

	if (mEnableBlending) {
		//加入blending
		auto src = color;
		auto dst = mFrameBuffer->mColorBuffer[pixelPos];
		float weight = static_cast<float>(src.mA) / 255.0f;

		result.mR = static_cast<float>(src.mR) * weight + static_cast<float>(dst.mR) * (1.0f - weight);
		result.mG = static_cast<float>(src.mG) * weight + static_cast<float>(dst.mG) * (1.0f - weight);
		result.mB = static_cast<float>(src.mB) * weight + static_cast<float>(dst.mB) * (1.0f - weight);
		result.mA = static_cast<float>(src.mA) * weight + static_cast<float>(dst.mA) * (1.0f - weight);
	}


	mFrameBuffer->mColorBuffer[pixelPos] = result;
}

void GPU::drawLine(const Point& p1, const Point& p2) {
	std::vector<Point> pixels;
	Raster::rasterizeLine(pixels, p1, p2);

	for (auto p : pixels) {
		drawPoint(p.x, p.y, p.color);
	}
}

void GPU::drawTriangle(const Point& p1, const Point& p2, const Point& p3) {
	std::vector<Point> pixels;
	Raster::rasterizeTriangle(pixels, p1, p2, p3);

	for (auto p : pixels) {
		drawPoint(p.x, p.y, p.color);
	}
}

void GPU::drawImage(const Image* image) {
	for (uint32_t i = 0; i < image->mWidth; ++i) {
		for (uint32_t j = 0; j < image->mHeight; ++j) {
			drawPoint(i, j, image->mData[j * image->mWidth + i]);
		}
	}
}

void GPU::drawImageWidthAlpha(const Image* image, const uint32_t& alpha) {
	RGBA color;
	for (uint32_t i = 0; i < image->mWidth; ++i) {
		for (uint32_t j = 0; j < image->mHeight; ++j) {
			color = image->mData[j * image->mWidth + i];
			color.mA = alpha;
			drawPoint(i, j, color);
		}
	}
}


//设置状态
void GPU::setBlending(bool enable) {
	mEnableBlending = enable;
}
