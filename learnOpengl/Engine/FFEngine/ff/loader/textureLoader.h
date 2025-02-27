#pragma once
#include "loader.h"
#include "../textures/texture.h"

namespace ff {

	class TextureLoader:public Loader {
	public:
		TextureLoader() noexcept;

		~TextureLoader() noexcept;

		//可以读取硬盘上的图片，或者读取已经拿到的图片数据流
		static Texture::Ptr load(const std::string& path, unsigned char* dataIn = nullptr, uint32_t widthIn = 0, uint32_t heightIn = 0);
	};
}