#pragma once
#include "../../global/base.h"

namespace ff {

	//整个渲染器在渲染过程中的各项指标数据记录
	class DriverInfo {
	public:

		struct Memory {
			uint32_t mGeometries{ 0 };
			uint32_t mTextures{ 0 };
		};

		struct Render {
			uint32_t	mFrame{ 0 };
			uint32_t	mCalls{ 0 };
			uint32_t	mTriangels{ 0 };
		};

		using Ptr = std::shared_ptr<DriverInfo>;
		static Ptr create() {
			return std::make_shared <DriverInfo>();
		}

		DriverInfo() noexcept;

		~DriverInfo() noexcept;

		void update(uint32_t count, uint32_t glMode, uint32_t instanceCount) noexcept;

		void reset() noexcept;

	public:
		Memory	mMemory{};
		Render	mRender{};
	};

}