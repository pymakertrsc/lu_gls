#include "renderTarget.h"
#include "../global/eventDispatcher.h"
#include "../tools/identity.h"

namespace ff {

	RenderTarget::RenderTarget(const uint32_t& width, const uint32_t& height, const Options& options) noexcept {
		mID = Identity::generateID();

		mWidth = width;
		mHeight = height;

		mTexture = Texture::create(
			width, 
			height, 
			options.mDataType, 
			options.mWrapS, 
			options.mWrapT, 
			options.mWrapR,
			options.mMagFilter,
			options.mMinFilter,
			options.mFormat);
		
		//本Texture是作为渲染输出目标点的用途，所以Source是nullptr
		mTexture->mUsage = TextureUsage::RenderTargetTexture;
		mTexture->mInternalFormat = options.mInternalFormat;

		mNeedsDepth = options.mNeedsDepthBuffer;
		mNeedsStencil = options.mNeedsStencilBuffer;

		mDepthTexture = options.mDepthTexture;

		mIsRenderTarget = true;
	}

	RenderTarget::~RenderTarget() noexcept {
		dispose();
	}


	void RenderTarget::setTexture(const Texture::Ptr& texture) noexcept {
		texture->mWidth = mWidth;
		texture->mHeight = mHeight;

		mTexture = texture;
	}

	void RenderTarget::setSize(const uint32_t& width, const uint32_t& height) noexcept {
		if (mWidth != width || mHeight != height) {
			mWidth = width;
			mHeight = height;

			mTexture->mWidth = width;
			mTexture->mHeight = height;

			dispose();
		}
	}

	void RenderTarget::dispose() noexcept {
		EventBase::Ptr e = EventBase::create("renderTargetDispose");
		e->mTarget = this;
		EventDispatcher::getInstance()->dispatchEvent(e);
	}
}