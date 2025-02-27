#include "driverInfo.h"

namespace ff {

	DriverInfo::DriverInfo() noexcept {}

	DriverInfo::~DriverInfo() noexcept {}

	void DriverInfo::update(uint32_t count, uint32_t glMode, uint32_t instanceCount) noexcept {
		mRender.mCalls++;

		switch (glMode) {
		case GL_TRIANGLES:
			mRender.mTriangels++;
			break;
		default:
			break;
		}
	}

	void DriverInfo::reset() noexcept {
		mRender.mFrame++;
		mRender.mCalls = 0;
		mRender.mTriangels = 0;
	}

}