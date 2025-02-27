#include "texture.h"
#include "../tools/identity.h"
#include "../global/eventDispatcher.h"

namespace ff {

	Texture::Texture(
		const uint32_t& width,
		const uint32_t& height,
		const DataType& dataType,
		const TextureWrapping& wrapS,
		const TextureWrapping& wrapT,
		const TextureWrapping& wrapR,
		const TextureFilter& magFilter,
		const TextureFilter& minFilter,
		const TextureFormat& format
	) noexcept {
		mID = Identity::generateID();
		mWidth = width;
		mHeight = height;
		mDataType = dataType;
		mWrapS = wrapS;
		mWrapR = wrapR;
		mWrapT = wrapT;
		mMagFilter = magFilter;
		mMinFilter = minFilter;
		mFormat = format;
		mTextureType = TextureType::Texture2D;
	}

	Texture::~Texture() noexcept {
		//������ʱ��ͨ��dispatcher���ⷢ����texture��������Ϣ
		EventBase::Ptr e = EventBase::create("textureDispose");
		e->mTarget = this;
		EventDispatcher::getInstance()->dispatchEvent(e);

		if (mSource) {
			EventBase::Ptr e = EventBase::create("sourceRelease");
			e->mTarget = mSource.get();
			EventDispatcher::getInstance()->dispatchEvent(e);
		}
	}

	Texture::Ptr Texture::clone() noexcept {
		auto texture = Texture::create(mWidth, mHeight, mDataType, mWrapS, mWrapT, mWrapR, mMagFilter, mMinFilter, mFormat);
		texture->mSource = mSource;
		texture->mUsage = mUsage;
		texture->mTextureType = mTextureType;
		texture->mInternalFormat = mInternalFormat;

		return texture;
	}

}