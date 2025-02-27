#include "renderableObject.h"

namespace ff {

	RenderableObject::RenderableObject(const Geometry::Ptr& geometry, const Material::Ptr& material) noexcept {
		mGeometry = geometry;
		mMaterial = material;

		mIsRenderableObject = true;
	}

	RenderableObject::~RenderableObject() noexcept {}

	void RenderableObject::onBeforeRender(Renderer* renderer, Scene* scene, Camera* camera) {
		if (mOnBeforeRenderCallback) {
			mOnBeforeRenderCallback(renderer, scene, camera);
		}
	}
}