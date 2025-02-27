#include "driverRenderTargets.h"
#include "../../global/eventDispatcher.h"

namespace ff {
	DriverRenderTarget::DriverRenderTarget() noexcept {}

	DriverRenderTarget::~DriverRenderTarget() noexcept {
		if (mFrameBuffer) {
			glDeleteFramebuffers(1, &mFrameBuffer);
		}

		if (mDepthRenderBuffer) {
			glDeleteRenderbuffers(1, &mDepthRenderBuffer);
		}
	}

	void DriverRenderTarget::generateFrameBuffer() noexcept {
		glGenFramebuffers(1, &mFrameBuffer);
	}

	DriverRenderTargets::DriverRenderTargets() noexcept {
		EventDispatcher::getInstance()->addEventListener("renderTargetDispose", this, &DriverRenderTargets::onRenderTargetDispose);
	}

	DriverRenderTargets::~DriverRenderTargets() noexcept {
		EventDispatcher::getInstance()->removeEventListener("renderTargetDispose", this, &DriverRenderTargets::onRenderTargetDispose);
	}

	DriverRenderTarget::Ptr DriverRenderTargets::get(const RenderTarget::Ptr& renderTarget) noexcept {
		auto id = renderTarget->mID;

		auto iter = mRenderTargets.find(id);
		if (iter == mRenderTargets.end()) {
			iter = mRenderTargets.insert(std::make_pair(id, DriverRenderTarget::create())).first;
		}

		return iter->second;
	}

	void DriverRenderTargets::onRenderTargetDispose(const EventBase::Ptr& e) {
		auto renderTarget = static_cast<RenderTarget*>(e->mTarget);
		auto iter = mRenderTargets.find(renderTarget->mID);
		if (iter == mRenderTargets.end()) {
			return;
		}

		mRenderTargets.erase(iter);
	}

}
