#include "cubeTextureLoader.h"
#include "cache.h"
#include <stb_image.h>
#include "../global/config.h"

namespace ff {

	CubeTextureLoader::CubeTextureLoader() noexcept {}

	CubeTextureLoader::~CubeTextureLoader() noexcept {}

	CubeTexture::Ptr CubeTextureLoader::load(const std::vector<std::string>& paths) noexcept {
		CubeTexture::Ptr texture = nullptr;
		auto filePaths = paths;

		//check file path
		assert(filePaths.size() == CubeTexture::CUBE_TEXTURE_COUNT);

		for (uint32_t i = 0; i < CubeTexture::CUBE_TEXTURE_COUNT; ++i) {
			auto path = filePaths[i];
			if (path.empty()) {
				filePaths[i] = DefaultTexturePath;
			}

			std::fstream file(path);
			if (!file.is_open()) {
				filePaths[i] = DefaultTexturePath;
			}
			else {
				file.close();
			}
		}

		//load images
		for (uint32_t i = 0; i < CubeTexture::CUBE_TEXTURE_COUNT; ++i) {
			auto filePath = filePaths[i];
			Source::Ptr source = Cache::getInstance()->getSource(filePath);

			if (source) {
				if (texture == nullptr) {
					texture = CubeTexture::create(source->mWidth, source->mHeight);
				}
				texture->mSources[i] = source;
			}
			else {
				source = Source::create();
				source->mNeedsUpdate = false;
				auto& data = source->mData;

				int			picType = 0;
				int width, height;

				try {
					unsigned char* bits = stbi_load(filePath.c_str(), &width, &height, &picType, toStbImageFormat(TextureFormat::RGBA));
					uint32_t dataSize = width * height * toByteSize(TextureFormat::RGBA);

					source->mWidth = width;
					source->mHeight = height;

					if (dataSize && bits) {
						data.resize(dataSize);
						memcpy(data.data(), bits, dataSize);
					}
					if (texture == nullptr) {
						texture = CubeTexture::create(source->mWidth, source->mHeight);
					}
					texture->mWidth = width;
					texture->mHeight = height;

					stbi_image_free(bits);
				}
				catch (std::exception e) {
					std::cout << e.what() << std::endl;
				}

				texture->mSources[i] = source;
				Cache::getInstance()->cacheSource(filePath, source);
			}
		}

		return texture;
	}
}