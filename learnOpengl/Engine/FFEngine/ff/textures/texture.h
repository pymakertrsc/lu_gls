#pragma once
#include "../global/base.h"
#include "../global/constant.h"
#include "source.h"

namespace ff {

	//负责给出纹理贴图描述信息
	class Texture {
	public:
		//智能指针管理
		using Ptr = std::shared_ptr<Texture>;
		static Ptr create(
			const uint32_t& width,
			const uint32_t& height,
			const DataType& dataType = DataType::UnsignedByteType,
			const TextureWrapping& wrapS = TextureWrapping::RepeatWrapping,
			const TextureWrapping& wrapT = TextureWrapping::RepeatWrapping,
			const TextureWrapping& wrapR = TextureWrapping::RepeatWrapping,
			const TextureFilter& magFilter = TextureFilter::LinearFilter,
			const TextureFilter& minFilter = TextureFilter::NearestFilter,
			const TextureFormat& format = TextureFormat::RGBA
		) {
			return std::make_shared <Texture>(
				width,
				height,
				dataType,
				wrapS,
				wrapT,
				wrapR,
				magFilter,
				minFilter,
				format
				);
		}

		Texture(
			const uint32_t& width,
			const uint32_t& height,
			const DataType& dataType = DataType::UnsignedByteType,
			const TextureWrapping& wrapS = TextureWrapping::RepeatWrapping,
			const TextureWrapping& wrapT = TextureWrapping::RepeatWrapping,
			const TextureWrapping& wrapR = TextureWrapping::RepeatWrapping,
			const TextureFilter& magFilter = TextureFilter::LinearFilter,
			const TextureFilter& minFilter = TextureFilter::LinearFilter,
			const TextureFormat& format = TextureFormat::RGBA
		) noexcept;

		~Texture() noexcept;

		ID getID() const noexcept { return mID; }

		TextureUsage getUsage() const noexcept { return mUsage; }

		virtual Texture::Ptr clone() noexcept;

	public:
		//Filter，Nearest四舍五入， 直接取得某个像素的颜色值
		//Filter，Linear取得周边像素的颜色值，按照远近程度标定权重，加权平均
		TextureFilter		mMinFilter{ TextureFilter::LinearFilter };
		TextureFilter		mMagFilter{ TextureFilter::LinearFilter };

		//包装方式，纹理坐标超出了0-1，怎么办？
		TextureWrapping		mWrapS{ TextureWrapping::RepeatWrapping };
		TextureWrapping		mWrapT{ TextureWrapping::RepeatWrapping };
		TextureWrapping		mWrapR{ TextureWrapping::RepeatWrapping };

		//像素格式信息
		//Format 即输入的图片像素格式，internalFormat即在shader采样的时候，内存中存放的格式
		TextureFormat		mFormat{ TextureFormat::RGBA };
		TextureFormat		mInternalFormat{ TextureFormat::RGBA };

		//每个颜色通道的数值格式
		DataType			mDataType{ DataType::UnsignedByteType };

		//必要的长宽
		uint32_t			mWidth{ 0 };
		uint32_t			mHeight{ 0 };

		Source::Ptr			mSource{ nullptr };

		//要么长宽变了，要么参数变了，要么数据变了
		bool				mNeedsUpdate{ true };

		//纹理类型，平面纹理，立方体贴图，纹理数组。。。
		TextureType			mTextureType{ TextureType::Texture2D };

		//本纹理用于何方，贴图，画布（colorRendertarget/colorAttachment）
		TextureUsage		mUsage{ TextureUsage::SamplerTexture };

	protected:
		ID	mID{ 0 };
	};
}