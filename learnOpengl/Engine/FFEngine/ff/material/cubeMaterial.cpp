#include "cubeMaterial.h"

namespace ff {

	CubeMaterial::CubeMaterial() noexcept {
		mType = MaterialName::CubeMaterial;
		mIsCubeMaterial = true;
		mDrawMode = DrawMode::Triangles;
	}

	CubeMaterial::~CubeMaterial() noexcept {}
}