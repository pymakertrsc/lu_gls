#pragma once
#include "../global/base.h"
#include "dataStructures.h"

class VertexArrayObject {
public:

	VertexArrayObject();
	~VertexArrayObject();

	void set(uint32_t binding, uint32_t vboId, size_t itemSize, size_t stride, size_t offset);

	std::map<uint32_t, BindingDescription> getBindingMap() const;

	void print();

private:
	//key:bindingId - value:bindingDescription
	std::map<uint32_t, BindingDescription> mBindingMap;
};