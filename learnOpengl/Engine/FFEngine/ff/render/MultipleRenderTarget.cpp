#include "MultipleRenderTarget.h"

namespace ff {

	MultipleRenderTarget::MultipleRenderTarget(const uint32_t& width, const uint32_t& height, const uint32_t& count, const RenderTarget::Options& options) noexcept :
		RenderTarget(width, height, options) {
		mIsMultiRenderTarget = true;

		//mTexture是其父类RenderTarget的哪个ColorAttachment
		//这个mTexture就作为了生成这多个ColorAttachments的模板
		for (uint32_t i = 0; i < count; ++i) {
			mTextures.push_back(mTexture->clone());
		}
	}

	//其父类已经调用了EventDispatcher
	MultipleRenderTarget::~MultipleRenderTarget() noexcept {}

	void MultipleRenderTarget::pushColor(const Texture::Ptr& texture) noexcept {
		mTextures.push_back(texture);
	}

	void MultipleRenderTarget::setSize(const uint32_t& width, const uint32_t& height) noexcept {
		if (mWidth != width || mHeight != height) {
			mWidth = width;
			mHeight = height;

			for (auto& texture : mTextures) {
				texture->mWidth = width;
				texture->mHeight = height;
			}

			dispose();
		}
	}
}