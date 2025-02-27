#include "depthTexture.h"

namespace ff {

	DepthTexture::DepthTexture(
		const uint32_t& width,
		const uint32_t& height,
		const DataType& dataType,
		const TextureWrapping& wrapS,
		const TextureWrapping& wrapT,
		const TextureWrapping& wrapR,
		const TextureFilter& magFilter,
		const TextureFilter& minFilter,
		const TextureFormat& format,
		const TextureType& textureType
	) noexcept:
	Texture(width, height, dataType, wrapS, wrapT, wrapR, magFilter, minFilter, format) {
		if (mFormat != TextureFormat::DepthFormat || mFormat != TextureFormat::DepthStencilFormat) {
			std::cout << "Error: DepthTexture format must be depthFormat or depthStencilFormat" << std::endl;
			return;
		}
		mInternalFormat = mFormat;
	}

	DepthTexture::~DepthTexture() noexcept {}
}