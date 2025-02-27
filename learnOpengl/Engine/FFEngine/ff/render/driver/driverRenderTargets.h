#pragma once
#include "../../global/base.h"
#include "../renderTarget.h"
#include "../../global/eventDispatcher.h"

namespace ff {

	class DriverRenderTarget {
	public:
		using Ptr = std::shared_ptr<DriverRenderTarget>;
		static Ptr create() { return std::make_shared<DriverRenderTarget>(); }

		DriverRenderTarget() noexcept;

		~DriverRenderTarget() noexcept;

		void generateFrameBuffer() noexcept;

	public:
		GLuint	mFrameBuffer{ 0 };

		//����û�û�����ⲿ����DepthTexture��˵���û�����ض���Ⱦ����������
		//�����û��Զ����DepthTexture������Ȩ�޹���DriverTextures
		// 
		//�������Ǳ����Լ�����һ��RenderBuffer�����;���RenderBuffer���ͣ����������Texture�������ƣ�Ҳ����
		// ���뵽mFrameBuffer������Ϊһ������
		// 
		// RenderBuffer��ʵ����һ�����飬���ñ����κ�Texture�ĸ�ʽ����OpenGL���ߵ��л������Ч��
		//
		GLuint	mDepthRenderBuffer{ 0 };
	};

	//����RenderTarget���䱾�ʻ���Texture��ص����ݣ�ֻ����ͨ��DriverRenderTarget����FrameBufferObject��������
	//ColorAttachment or DepthAttachment�Ľ�������Ȼ����Texture��صĽ������У����Խ���RenderTarget������ͽ�����DriverTextures
	class DriverRenderTargets {
	public:
		using Ptr = std::shared_ptr<DriverRenderTargets>;
		static Ptr create() { return std::make_shared<DriverRenderTargets>(); }

		DriverRenderTargets() noexcept;

		~DriverRenderTargets() noexcept;

		DriverRenderTarget::Ptr get(const RenderTarget::Ptr& renderTarget) noexcept;

		void onRenderTargetDispose(const EventBase::Ptr& e);
		
	private:
		std::unordered_map<ID, DriverRenderTarget::Ptr> mRenderTargets{};
	};
}
