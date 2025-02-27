#pragma once
#include "loader.h"
#include "../textures/cubeTexture.h"

namespace ff {

	class CubeTextureLoader:public Loader {
	public:
		CubeTextureLoader() noexcept;

		~CubeTextureLoader() noexcept;

		static CubeTexture::Ptr load(const std::vector<std::string>& paths) noexcept;
	};
}