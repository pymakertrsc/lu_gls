#pragma once
#include "material.h"

namespace ff {

	class DepthMaterial :public Material {
	public:
		static constexpr uint32_t	NoPacking = 0;
		static constexpr uint32_t	RGBADepthPacking = 1;

		using Ptr = std::shared_ptr<DepthMaterial>;
		static Ptr create(uint32_t packing = NoPacking) { return std::make_shared<DepthMaterial>(packing); }

		DepthMaterial(uint32_t packing) noexcept;

		~DepthMaterial() noexcept;

	public:
		//�Ƿ�������ȴ��
		uint32_t mPacking{ NoPacking };
	};
}