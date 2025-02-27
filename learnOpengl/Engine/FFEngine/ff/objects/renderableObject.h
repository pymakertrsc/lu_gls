#pragma once
#include "../core/object3D.h"
#include "../core/geometry.h"
#include "../material/material.h"
#include "../global/base.h"
#include "../global/constant.h"

namespace ff {

	class Renderer;
	class Scene;
	class Camera;

	//一切可渲染物体的基类
	class RenderableObject :public Object3D {
	public:
		using OnBeforeRenderCallback = std::function<void(Renderer*, Scene*, Camera*)>;

		//使用智能指针进行管理
		using Ptr = std::shared_ptr<RenderableObject>;
		static Ptr create(const Geometry::Ptr& geometry, const Material::Ptr& material) {
			return std::make_shared <RenderableObject>(geometry, material);
		}

		RenderableObject(const Geometry::Ptr& geometry, const Material::Ptr& material) noexcept;

		~RenderableObject() noexcept;

		auto getGeometry() const noexcept { return mGeometry; }

		auto getMaterial() const noexcept { return mMaterial; }

		//在本物体渲染前，会调用本函数，允许用户指定渲染前做哪些处理
		void onBeforeRender(Renderer* renderer, Scene* scene, Camera* camera);

	public:
		//在本可绘制物体，进行渲染之前，会回调这个函数进行通知
		OnBeforeRenderCallback mOnBeforeRenderCallback{ nullptr };

	protected:
		Geometry::Ptr mGeometry{ nullptr };
		Material::Ptr mMaterial{ nullptr };
	};
}