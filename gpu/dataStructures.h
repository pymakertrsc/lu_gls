#pragma once
#include "../global/base.h"
#include "../math/math.h"

//VAO之中，用于描述属性读取方式的Description
struct BindingDescription {
	uint32_t	mVboId{ 0 };
	size_t		mItemSize{ 0 };
	size_t		mStride{ 0 };
	size_t		mOffset{ 0 };
};

struct VsOutput {
	float mOneOverW{ 0.0f };
	math::vec4f mPosition{ 0.0f, 0.0f, 0.0f, 1.0f };
	math::vec4f mColor;//此处颜色改为0.0-1.0之间表达0-255的量
	math::vec3f mNormal;
	math::vec2f mUV;
};

struct FsOutput {
	math::vec2i mPixelPos;
	float mDepth;
	RGBA mColor;//此处使用0-255来进行颜色显示
};
