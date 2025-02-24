#pragma once
#include "../global/base.h"

class BufferObject {
public:
	BufferObject();
	~BufferObject();
	BufferObject(const BufferObject&) = delete;

	
	void setBufferData(size_t dataSize, void* data);
	byte* getBuffer() const;
	size_t getSize() const;

private:
	size_t	mBufferSize{ 0 };//bytes
	byte*	mBuffer{ nullptr };
};