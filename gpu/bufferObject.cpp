#include"bufferObject.h"

BufferObject::BufferObject() {}

BufferObject::~BufferObject() {
	if (mBuffer) {
		delete[] mBuffer;
	}
}

void BufferObject::setBufferData(size_t dataSize, void* data) {
	if (mBuffer != nullptr && mBufferSize < dataSize) {
		delete []mBuffer;
		mBuffer = nullptr;
	}

	if (mBuffer == nullptr) {
		mBuffer = new byte[dataSize];
		mBufferSize = dataSize;
	}

	memcpy(mBuffer, data, dataSize);
}

byte* BufferObject::getBuffer() const {
	return mBuffer;
}

size_t BufferObject::getSize() const {
	return mBufferSize;
}
