#pragma once
#include "renderTarget.h"

namespace ff {

	class DriverTextures;
	//ӵ�ж����ɫ���ͨ��
	class MultipleRenderTarget :public RenderTarget {
	public:
		friend DriverTextures;

		using Ptr = std::shared_ptr<MultipleRenderTarget>;
		static Ptr create(const uint32_t& width, const uint32_t& height, const uint32_t& count, const RenderTarget::Options& options) {
			return std::make_shared <MultipleRenderTarget>(width, height, count, options);
		}

		//count���ж��ٸ�ColorAttachment
		MultipleRenderTarget(const uint32_t& width, const uint32_t& height, const uint32_t& count, const RenderTarget::Options& options) noexcept;
		
		~MultipleRenderTarget() noexcept;

		void pushColor(const Texture::Ptr& texture) noexcept;

		virtual void setSize(const uint32_t& width, const uint32_t& height) noexcept override;

	private:
		std::vector<Texture::Ptr> mTextures{};
	};
}