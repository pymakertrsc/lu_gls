#include "cubeTexture.h"
#include "../global/eventDispatcher.h"

namespace ff {

	CubeTexture::CubeTexture(
		const uint32_t& width,
		const uint32_t& height,
		const DataType& dataType,
		const TextureWrapping& wrapS,
		const TextureWrapping& wrapT,
		const TextureWrapping& wrapR,
		const TextureFilter& magFilter,
		const TextureFilter& minFilter,
		const TextureFormat& format
	) noexcept: Texture(width, height, dataType, wrapS, wrapT, wrapR, magFilter, minFilter, format) {
		mTextureType = TextureType::TextureCubeMap;
	}

	CubeTexture::~CubeTexture() noexcept {
		for (uint32_t i = 0; i < CUBE_TEXTURE_COUNT; ++i) {
			auto source = mSources[i];
			if (source) {
				EventBase::Ptr e = EventBase::create("sourceRelease");
				e->mTarget = source.get();
				EventDispatcher::getInstance()->dispatchEvent(e);
			}
		}
	}
}