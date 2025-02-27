#pragma once
#include "../global/base.h"
#include "../global/constant.h"
#include "source.h"

namespace ff {

	//�������������ͼ������Ϣ
	class Texture {
	public:
		//����ָ�����
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
		//Filter��Nearest�������룬 ֱ��ȡ��ĳ�����ص���ɫֵ
		//Filter��Linearȡ���ܱ����ص���ɫֵ������Զ���̶ȱ궨Ȩ�أ���Ȩƽ��
		TextureFilter		mMinFilter{ TextureFilter::LinearFilter };
		TextureFilter		mMagFilter{ TextureFilter::LinearFilter };

		//��װ��ʽ���������곬����0-1����ô�죿
		TextureWrapping		mWrapS{ TextureWrapping::RepeatWrapping };
		TextureWrapping		mWrapT{ TextureWrapping::RepeatWrapping };
		TextureWrapping		mWrapR{ TextureWrapping::RepeatWrapping };

		//���ظ�ʽ��Ϣ
		//Format �������ͼƬ���ظ�ʽ��internalFormat����shader������ʱ���ڴ��д�ŵĸ�ʽ
		TextureFormat		mFormat{ TextureFormat::RGBA };
		TextureFormat		mInternalFormat{ TextureFormat::RGBA };

		//ÿ����ɫͨ������ֵ��ʽ
		DataType			mDataType{ DataType::UnsignedByteType };

		//��Ҫ�ĳ���
		uint32_t			mWidth{ 0 };
		uint32_t			mHeight{ 0 };

		Source::Ptr			mSource{ nullptr };

		//Ҫô������ˣ�Ҫô�������ˣ�Ҫô���ݱ���
		bool				mNeedsUpdate{ true };

		//�������ͣ�ƽ��������������ͼ���������顣����
		TextureType			mTextureType{ TextureType::Texture2D };

		//���������ںη�����ͼ��������colorRendertarget/colorAttachment��
		TextureUsage		mUsage{ TextureUsage::SamplerTexture };

	protected:
		ID	mID{ 0 };
	};
}