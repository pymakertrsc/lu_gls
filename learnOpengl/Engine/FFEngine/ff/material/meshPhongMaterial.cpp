#include "meshPhongMaterial.h"

namespace ff {

	MeshPhongMaterial::MeshPhongMaterial() noexcept {
		mType = MaterialName::MeshPhongMaterial;
		mIsMeshPhongMaterial = true;
		mDrawMode = DrawMode::Triangles;
	}

	MeshPhongMaterial::~MeshPhongMaterial() noexcept {}
}