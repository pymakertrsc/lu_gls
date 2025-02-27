#pragma once
#include "material.h"

namespace ff {

	class MeshPhongMaterial :public Material {
	public:
		using Ptr = std::shared_ptr<MeshPhongMaterial>;
		static Ptr create() { return std::make_shared<MeshPhongMaterial>(); }

		MeshPhongMaterial() noexcept;

		~MeshPhongMaterial() noexcept;

		float mShininess{ 32.0f };
	};
}