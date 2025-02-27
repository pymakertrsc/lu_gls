#pragma once
#include "loader.h"
#include "../textures/texture.h"

namespace ff {

	class TextureLoader:public Loader {
	public:
		TextureLoader() noexcept;

		~TextureLoader() noexcept;

		//���Զ�ȡӲ���ϵ�ͼƬ�����߶�ȡ�Ѿ��õ���ͼƬ������
		static Texture::Ptr load(const std::string& path, unsigned char* dataIn = nullptr, uint32_t widthIn = 0, uint32_t heightIn = 0);
	};
}