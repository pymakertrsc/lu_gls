#include "texture.h"
#include "../math/math.h"

Texture::Texture() {}
Texture::~Texture() {
	if (mBuffer != nullptr) {
		delete[] mBuffer;
		mBuffer = nullptr;
	}
}

void Texture::setBufferData(
	const uint32_t& width,
	const uint32_t& height,
	void* data) {
	if (mBuffer != nullptr) {
		delete[] mBuffer;
		mBuffer = nullptr;
	}

	mWidth = width;
	mHeight = height;

	if (mBuffer == nullptr) {
		mBuffer = new RGBA[mWidth * mHeight];
	}

	memcpy(mBuffer, data, mWidth * mHeight * sizeof(RGBA));
}

math::vec4f Texture::getColor(float u, float v) {
	RGBA resultColor;

	//处理uv坐标
	checkWrap(u,mWrapU);
	checkWrap(v,mWrapV);

	if (mFilter == TEXTURE_FILTER_NEAREST) {
		int x = std::round(u * (mWidth - 1));
		int y = std::round(v * (mHeight - 1));

		int position = y * mWidth + x;
		resultColor = mBuffer[position];
	}
	else if (mFilter == TEXTURE_FILTER_LINEAR) {
		float x = u * static_cast<float>(mWidth - 1);
		float y = v * static_cast<float>(mHeight - 1);

		int left = std::floor(x);
		int right = std::ceil(x);
		int bottom = std::floor(y);
		int top = std::ceil(y);

		//对上下插值，得到左右
		float yScale = 0.0f;
		if (top == bottom) {
			yScale = 1.0f;
		}
		else {
			yScale = (y - static_cast<float>(bottom)) / static_cast<float>(top - bottom);
		}

		int positionLeftTop = top * mWidth + left;
		int positionLeftBottom = bottom * mWidth + left;
		int positionRightTop = top * mWidth + right;
		int positionRightBottom = bottom * mWidth + right;

		RGBA leftColor = math::lerp(mBuffer[positionLeftBottom], mBuffer[positionLeftTop], yScale);
		RGBA rightColor = math::lerp(mBuffer[positionRightBottom], mBuffer[positionRightTop], yScale);

		//对左右插值，得到结果
		float xScale = 0.0f;
		if (right == left) {
			xScale = 1.0f;
		}
		else {
			xScale = (x - static_cast<float>(left)) / static_cast<float>(right - left);
		}

		resultColor = math::lerp(leftColor, rightColor, xScale);
	}

	math::vec4f result;
	result.x = static_cast<float>(resultColor.mR) / 255.0;
	result.y = static_cast<float>(resultColor.mG) / 255.0;
	result.z = static_cast<float>(resultColor.mB) / 255.0;
	result.w = static_cast<float>(resultColor.mA) / 255.0;

	return result;
}

void Texture::checkWrap(float& n, const uint32_t& type) {
	if (n > 1.0f || n < 0.0f) {
		n = FRACTION(n);

		switch (type) {
		case TEXTURE_WRAP_REPEAT:
			n = FRACTION(n + 1);
			break;
		case TEXTURE_WRAP_MIRROR:
			n = 1.0f - FRACTION(n + 1);
			break;
		default:
			break;
		}
	}

}

void Texture::setParameter(const uint32_t& type, const uint32_t& value) {
	switch (type) {
	case TEXTURE_FILTER:
		mFilter = value;
		break;
	case TEXTURE_WRAP_U:
		mWrapU = value;
		break;
	case TEXTURE_WRAP_V:
		mWrapV = value;
		break;
	default:
		break;
	}
}