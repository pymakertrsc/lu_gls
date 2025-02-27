#include "depthMaterial.h"

namespace ff {

	DepthMaterial::DepthMaterial(uint32_t packing) noexcept {
		mType = MaterialName::DepthMaterial;
		mIsDepthMaterial = true;
		mDrawMode = DrawMode::Triangles;
		mPacking = packing;
	}

	DepthMaterial::~DepthMaterial() noexcept {}
}