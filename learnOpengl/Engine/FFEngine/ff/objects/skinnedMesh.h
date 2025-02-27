#pragma once 
#include "../global/base.h"
#include "skeleton.h"
#include "mesh.h"

namespace ff {

	//骨骼动画相关的Mesh
	class SkinnedMesh :public Mesh {
	public:
		using Ptr = std::shared_ptr<SkinnedMesh>;
		static Ptr create(const Geometry::Ptr& geometry, const Material::Ptr& material) {
			return std::make_shared<SkinnedMesh>(geometry, material);
		}

		SkinnedMesh(const Geometry::Ptr& geometry, const Material::Ptr& material) noexcept;

		~SkinnedMesh() noexcept;

		void bind(const Skeleton::Ptr& skeleton) noexcept;

	public:
		Skeleton::Ptr	mSkeleton{ nullptr };
	};
}