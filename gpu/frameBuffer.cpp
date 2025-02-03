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

}

FrameBuffer::~FrameBuffer() {
	if (!mExternBuffer && mColorBuffer) {
		delete[] mColorBuffer;
	}
}
