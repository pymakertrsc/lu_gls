#pragma once
#include "../../global/base.h"
#include "../../textures/texture.h"
#include "../../textures/cubeTexture.h"
#include "../../global/eventDispatcher.h"
#include "driverInfo.h"
#include "../renderTarget.h"
#include "driverRenderTargets.h"

namespace ff {

	class DriverTexture {
	public:
		using Ptr = std::shared_ptr<DriverTexture>;
		static Ptr create() {
			return std::make_shared <DriverTexture>();
		}

		DriverTexture() noexcept;

		~DriverTexture() noexcept;

		void dispose() noexcept;

		//ͨ��glGenTextures��õ�texture�ı��
		GLuint	mHandle{ 0 };

	};
	
	/*
	* ����ÿһ��texture��Ҫȷ������ʽʹ�õ�ʱ�򣬲��ܽ��н���
	* ����ж�һ��texture��ʽʹ�����أ�
	* ��texture����Ϊuniform����upload��ʱ�򣬲�������ʽʹ�õ�����
	*/
	class DriverTextures {
	public:
		using Ptr = std::shared_ptr<DriverTextures>;
		static Ptr create(const DriverInfo::Ptr& info, const DriverRenderTargets::Ptr& renderTargets) {
			return std::make_shared <DriverTextures>(info, renderTargets);
		}

		DriverTextures(const DriverInfo::Ptr& info, const DriverRenderTargets::Ptr& renderTargets) noexcept;

		~DriverTextures() noexcept;

		//����texture��������Ӧ��DriverTexture
		DriverTexture::Ptr get(const Texture::Ptr& texture) noexcept;

		//����:
		// ��texture��Ӧ��Driver Texture���е�mHandle���󶨵�textureUnit��slot��
		//GL_TEXTURE0 GL_TEXTURE1....
		void bindTexture(const Texture::Ptr& texture, GLenum textureUnit);

		void setupRenderTarget(const RenderTarget::Ptr& renderTarget) noexcept;

		void onTextureDestroy(const EventBase::Ptr& e) noexcept;

	private:
		//Ҫô�½�һ��texture �� Ҫô����ԭ��texture���������ݻ�����������
		void update(const Texture::Ptr& texture) noexcept;

		DriverTexture::Ptr setupDriverTexture(const Texture::Ptr& texture) noexcept;

		void setupFBOColorAttachment(const GLuint& fbo, const GLenum& target, const Texture::Ptr& texture) noexcept;

		void setupFBODepthStencilAttachment(const RenderTarget::Ptr& renderTarget) noexcept;

		void setupDepthTexture(const GLuint& frameBuffer, const RenderTarget::Ptr& renderTarget) noexcept;

		void setupDepthRenderBuffer(const GLuint& frameBuffer, const RenderTarget::Ptr& renderTarget);

	private:
		DriverInfo::Ptr								mInfo{ nullptr };
		DriverRenderTargets::Ptr					mRenderTargets{ nullptr };
		std::unordered_map<ID, DriverTexture::Ptr>	mTextures{};
	};
}