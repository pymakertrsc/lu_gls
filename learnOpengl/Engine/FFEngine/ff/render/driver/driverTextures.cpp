#include "driverTextures.h"
#include "../MultipleRenderTarget.h"

namespace ff {

	DriverTexture::DriverTexture() noexcept {
	}

	DriverTexture::~DriverTexture() noexcept {
		dispose();
	}

	void DriverTexture::dispose() noexcept { 
		if (mHandle) {
			glDeleteTextures(GL_TEXTURE_2D, &mHandle);
			mHandle = 0;
		}
	}

	DriverTextures::DriverTextures(const DriverInfo::Ptr& info, const DriverRenderTargets::Ptr& renderTargets) noexcept {
		mInfo = info;
		mRenderTargets = renderTargets;

		EventDispatcher::getInstance()->addEventListener("textureDispose", this, &DriverTextures::onTextureDestroy);
	}

	DriverTextures::~DriverTextures() noexcept {
		EventDispatcher::getInstance()->removeEventListener("textureDispose", this, &DriverTextures::onTextureDestroy);
	}

	void DriverTextures::update(const Texture::Ptr& texture) noexcept {
		auto dTexture = get(texture);

		//mNeedsUpdate在texture初次创建的时候，会是true
		//在使用者更改了texture相关的东西之后，可以手动将其置为true，就会触发更改
		if (texture->mNeedsUpdate) {
			texture->mNeedsUpdate = false;
			setupDriverTexture(texture);
		}

	}

	DriverTexture::Ptr DriverTextures::setupDriverTexture(const Texture::Ptr& texture) noexcept {
		DriverTexture::Ptr dtexture = get(texture);
		texture->mNeedsUpdate = false;

		if (!dtexture->mHandle) {
			glGenTextures(1, &dtexture->mHandle);
		}
		glBindTexture(toGL(texture->mTextureType), dtexture->mHandle);

		//设置纹理参数
		glTexParameteri(toGL(texture->mTextureType), GL_TEXTURE_MIN_FILTER, toGL(texture->mMinFilter));
		glTexParameteri(toGL(texture->mTextureType), GL_TEXTURE_MAG_FILTER, toGL(texture->mMagFilter));
		glTexParameteri(toGL(texture->mTextureType), GL_TEXTURE_WRAP_S, toGL(texture->mWrapS));
		glTexParameteri(toGL(texture->mTextureType), GL_TEXTURE_WRAP_T, toGL(texture->mWrapT));
		glTexParameteri(toGL(texture->mTextureType), GL_TEXTURE_WRAP_R, toGL(texture->mWrapR));

		if (texture->mTextureType == TextureType::Texture2D) {
			//必须是贴图专用的texture而不是渲染目标，才可能有图片数据
			const byte* data = (texture->getUsage() == TextureUsage::SamplerTexture) ? texture->mSource->mData.data() : nullptr;

			//1 开辟内存空间
			//2 传输图片数据
			glTexImage2D(GL_TEXTURE_2D, 0, toGL(texture->mInternalFormat), texture->mWidth, texture->mHeight, 0, toGL(texture->mFormat), toGL(texture->mDataType), data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {

			//为当前的cubeMap的texture做六次内存开辟以及数据更新
			for (uint32_t i = 0; i < CubeTexture::CUBE_TEXTURE_COUNT; ++i) {
				auto cubeTexture = std::static_pointer_cast<CubeTexture>(texture);
				const byte* data = (texture->getUsage() == TextureUsage::SamplerTexture) ? cubeTexture->mSources[i]->mData.data() : nullptr;

				//开辟内存及更新数据的顺序：右左上下前后
				//要给哪一个面开辟内存更新数据，就输入哪一个面的target
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, toGL(texture->mInternalFormat), texture->mWidth, texture->mHeight, 0, toGL(texture->mFormat), toGL(texture->mDataType), data);
			}
		}

		glBindTexture(toGL(texture->mTextureType), 0);
		mInfo->mMemory.mTextures++;

		return dtexture;
	}

	void DriverTextures::setupFBOColorAttachment(const GLuint& fbo, const GLenum& attachment, const Texture::Ptr& texture) noexcept {
		auto dTexture = get(texture);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, toGL(texture->mTextureType), dTexture->mHandle, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void DriverTextures::setupFBODepthStencilAttachment(const RenderTarget::Ptr& renderTarget) noexcept {
		auto dRenderTarget = mRenderTargets->get(renderTarget);
		if (!dRenderTarget->mFrameBuffer) {
			std::cout << "Error: frameBuffer has not been generated,when setupFBODepthStencilAttachment!" << std::endl;
			return;
		}

		//TODO: support stencil buffer
		//如果需要DepthTest，并且用户指定了一张用户创建的Texture
		if (renderTarget->mNeedsDepth && renderTarget->mDepthTexture) {
			setupDepthTexture(dRenderTarget->mFrameBuffer, renderTarget);
		}
		//如果用户没有指定，自己默认创建一个用于深度检测的RenderBuffer
		else if (renderTarget->mNeedsDepth) {
			setupDepthRenderBuffer(dRenderTarget->mFrameBuffer, renderTarget);
		}

	}

	void DriverTextures::setupDepthTexture(const GLuint& frameBuffer, const RenderTarget::Ptr& renderTarget) noexcept {
		if (!renderTarget->mDepthTexture || !renderTarget->mNeedsDepth) {
			std::cout << "Error: renderTarget has no depth info when setupDepthTexture";
			return;
		}
		auto depthTexture = renderTarget->mDepthTexture;
		auto dDepthTexture = get(depthTexture);
		setupDriverTexture(depthTexture);

		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, toGL(depthTexture->mTextureType), dDepthTexture->mHandle, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void DriverTextures::setupDepthRenderBuffer(const GLuint& frameBuffer, const RenderTarget::Ptr& renderTarget) {
		auto dRenderTarget = mRenderTargets->get(renderTarget);
	
		//创建RenderBuffer
		glGenRenderbuffers(1, &dRenderTarget->mDepthRenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, dRenderTarget->mDepthRenderBuffer);

		//为renderBuffer对象开辟空间
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, renderTarget->mWidth, renderTarget->mHeight);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);


		//向frameBuffer进行绑定
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, dRenderTarget->mDepthRenderBuffer);

		GLint error = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	DriverTexture::Ptr DriverTextures::get(const Texture::Ptr& texture) noexcept {
		auto iter = mTextures.find(texture->getID());
		if (iter == mTextures.end()) {
			iter = mTextures.insert(std::make_pair(texture->getID(), DriverTexture::create())).first;
		}

		return iter->second;
	}

	void DriverTextures::bindTexture(const Texture::Ptr& texture, GLenum textureUnit) {
		//GL_TEXTURE0  GL_TEXTURE1 GL_TEXTURE2....
		//GL_TEXTURE1 = GL_TEXTURE0+1
		//GL_TEXTURE2 = GL_TEXTURE0+2
		glActiveTexture(textureUnit);
		//更新或者创建textureID
		update(texture);
		auto dTexture = get(texture);
		glBindTexture(toGL(texture->mTextureType), dTexture->mHandle);
	}

	void DriverTextures::setupRenderTarget(const RenderTarget::Ptr& renderTarget) noexcept {
		auto dRenderTarget = mRenderTargets->get(renderTarget);

		//generate framebuffer
		if (!dRenderTarget->mFrameBuffer) {
			dRenderTarget->generateFrameBuffer();
		}

		//setup color attachments
		if (renderTarget->mIsMultiRenderTarget) {
			auto multiRT = std::static_pointer_cast<MultipleRenderTarget>(renderTarget);
			auto textures = multiRT->mTextures;

			for (uint32_t i = 0; i < textures.size(); ++i) {
				auto texture = textures[i];
				setupDriverTexture(texture);
				setupFBOColorAttachment(dRenderTarget->mFrameBuffer, GL_COLOR_ATTACHMENT0 + i, texture);
			}
		}
		else {
			auto texture = renderTarget->mTexture;
			setupDriverTexture(texture);
			setupFBOColorAttachment(dRenderTarget->mFrameBuffer, GL_COLOR_ATTACHMENT0, texture);
		}

		//setup depth and stencil
		if (renderTarget->mNeedsDepth) {
			setupFBODepthStencilAttachment(renderTarget);
		}
	}

	void DriverTextures::onTextureDestroy(const EventBase::Ptr& e) noexcept {
		auto texture = static_cast<Texture*>(e->mTarget);
		auto id = texture->getID();

		auto iter = mTextures.find(texture->getID());
		if (iter != mTextures.end()) {
			mTextures.erase(iter);
			mInfo->mMemory.mTextures--;
		}
	}
}