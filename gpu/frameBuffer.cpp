#include "frameBuffer.h"

FrameBuffer::FrameBuffer(uint32_t width, uint32_t height, void* buffer) {
	mWidth = width;
	mHeight = height;

	if (!buffer) {
		buffer = new RGBA[width * height];
		mExternBuffer = false;
	}
	else {
		mExternBuffer = true;
	}

	mColorBuffer = (RGBA*)buffer;

	mDepthBuffer = new float[width * height];
	std::fill_n(mDepthBuffer, width * height, 1.0f);
}

FrameBuffer::~FrameBuffer() {
	if (!mExternBuffer && mColorBuffer) {
		delete[] mColorBuffer;
	}

	if (mDepthBuffer) {
		delete[] mDepthBuffer;
	}
}
