#pragma once
#include "../../global/base.h"
#include "../../camera/camera.h"
#include "../../scene/scene.h"
#include "../../lights/light.h"
#include "../../material/depthMaterial.h"

namespace ff {

	class DriverRenderState;
	class DriverObjects;
	class Renderer;
	class DriverState;
	class DriverShadowMap {
	public:
		using Ptr = std::shared_ptr<DriverShadowMap>;
		static Ptr create(Renderer* renderer, const std::shared_ptr<DriverObjects>& objects, const std::shared_ptr<DriverState>& state) {
			return std::make_shared<DriverShadowMap>(renderer, objects, state);
		}

		DriverShadowMap(Renderer* renderer, const std::shared_ptr<DriverObjects>& objects, const std::shared_ptr<DriverState>& state) noexcept;

		~DriverShadowMap() noexcept;

		void render(const std::shared_ptr<DriverRenderState>& renderState, const Scene::Ptr& scene, const Camera::Ptr& camera) noexcept;

		void renderObject(
			const Object3D::Ptr& object,
			const Camera::Ptr& camera,
			const Camera::Ptr& shadowCamera,
			const Light::Ptr& light,
			const Frustum::Ptr& frustum) noexcept;

	public:
		//决定整个系统是否开启ShadowMap
		bool mEnabled{ true };

	private:
		Renderer* mRenderer{ nullptr };
		std::shared_ptr<DriverObjects>	mObjects{ nullptr };
		std::shared_ptr<DriverState>	mState{ nullptr };

		DepthMaterial::Ptr	mDefaultDepthMaterial = DepthMaterial::create(DepthMaterial::RGBADepthPacking);
	};
}