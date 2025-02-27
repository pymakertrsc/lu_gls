#include "mesh.h"

namespace ff {

	Mesh::Mesh(const Geometry::Ptr& geometry, const Material::Ptr& material) noexcept
		:RenderableObject(geometry, material){
		mIsMesh = true;
	}

	Mesh::~Mesh() noexcept {}
}