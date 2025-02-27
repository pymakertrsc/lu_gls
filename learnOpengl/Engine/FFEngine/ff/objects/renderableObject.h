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

	//һ�п���Ⱦ����Ļ���
	class RenderableObject :public Object3D {
	public:
		using OnBeforeRenderCallback = std::function<void(Renderer*, Scene*, Camera*)>;

		//ʹ������ָ����й���
		using Ptr = std::shared_ptr<RenderableObject>;
		static Ptr create(const Geometry::Ptr& geometry, const Material::Ptr& material) {
			return std::make_shared <RenderableObject>(geometry, material);
		}

		RenderableObject(const Geometry::Ptr& geometry, const Material::Ptr& material) noexcept;

		~RenderableObject() noexcept;

		auto getGeometry() const noexcept { return mGeometry; }

		auto getMaterial() const noexcept { return mMaterial; }

		//�ڱ�������Ⱦǰ������ñ������������û�ָ����Ⱦǰ����Щ����
		void onBeforeRender(Renderer* renderer, Scene* scene, Camera* camera);

	public:
		//�ڱ��ɻ������壬������Ⱦ֮ǰ����ص������������֪ͨ
		OnBeforeRenderCallback mOnBeforeRenderCallback{ nullptr };

	protected:
		Geometry::Ptr mGeometry{ nullptr };
		Material::Ptr mMaterial{ nullptr };
	};
}