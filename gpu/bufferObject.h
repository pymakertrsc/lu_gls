#pragma once
#include "../global/base.h"

class BufferObject {
public:
	BufferObject();
	~BufferObject();
	BufferObject(const BufferObject&) = delete;

	//只有调用本函数的时候，才进行内存空间的分配
	void setBufferData(size_t dataSize, void* data);
	byte* getBuffer() const;
	size_t getSize() const;

private:
	size_t	mBufferSize{ 0 };//bytes
	byte*	mBuffer{ nullptr };
};