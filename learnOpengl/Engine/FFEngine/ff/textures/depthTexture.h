#pragma once
#include "texture.h"

namespace ff {

	class DepthTexture :public Texture {
	public:
		using Ptr = std::shared_ptr<DepthTexture>;
		static Ptr create(
			const uint32_t& width,
			const uint32_t& height,
			const DataType& dataType = DataType::FloatType,
			const TextureWrapping& wrapS = TextureWrapping::RepeatWrapping,
			const TextureWrapping& wrapT = TextureWrapping::RepeatWrapping,
			const TextureWrapping& wrapR = TextureWrapping::RepeatWrapping,
			const TextureFilter& magFilter = TextureFilter::NearestFilter,
			const TextureFilter& minFilter = TextureFilter::NearestFilter,
			const TextureFormat& format = TextureFormat::DepthFormat,
			const TextureType& textureType = TextureType::Texture2D)
		{
			return std::make_shared <DepthTexture>(
				width, 
				height,
				dataType,
				wrapS,
				wrapT,
				wrapR,
				magFilter,
				minFilter,
				format,
				textureType
				);
		}

		DepthTexture(
			const uint32_t& width,
			const uint32_t& height,
			const DataType& dataType = DataType::FloatType,
			const TextureWrapping& wrapS = TextureWrapping::RepeatWrapping,
			const TextureWrapping& wrapT = TextureWrapping::RepeatWrapping,
			const TextureWrapping& wrapR = TextureWrapping::RepeatWrapping,
			const TextureFilter& magFilter = TextureFilter::NearestFilter,
			const TextureFilter& minFilter = TextureFilter::NearestFilter,
			const TextureFormat& format = TextureFormat::DepthFormat,
			const TextureType& textureType = TextureType::Texture2D) noexcept;

		~DepthTexture() noexcept;

	};
}