#include "driverBackground.h"
#include "../../geometries/boxGeometry.h"
#include "../../material/cubeMaterial.h"
#include "../../camera/camera.h"
#include "../renderer.h"

namespace ff {

	DriverBackground::DriverBackground(Renderer* renderer, const DriverObjects::Ptr& objects) noexcept {
		mRenderer = renderer;
		mObjects = objects;
	}

	DriverBackground::~DriverBackground() noexcept {
	}

	void DriverBackground::render(const DriverRenderList::Ptr& renderList, const Scene::Ptr& scene) {
		//clear renderer
		if (mRenderer->mAutoClear) {
			mRenderer->clear();
		}

		//prepare background
		//��CubeTextureȡ�� ,��ֵ��background
		auto background = scene->mBackground;
		if (background == nullptr) {
			return;
		}

		//�������ڻ�����պУ�CubeMap����������Mesh
		if (mBoxMesh == nullptr) {
			//����һ��1*1*1 ��boxGeometry
			auto geometry = BoxGeometry::create(1.0f, 1.0f, 1.0f);
			geometry->deleteAttribute("normal");
			geometry->deleteAttribute("uv");

			//�������Զ����box���ڲ����ģ�����ֻ�����ڲ�����
			auto material = CubeMaterial::create();
			material->mSide = Side::BackSide;
			material->mEnvMap = background;

			mBoxMesh = Mesh::create(geometry, material);

			//��ǰ��mBoxMesh����Ȼ������������ϵ�ģ�000��λ�ã����box��Ҫ����Camera�ƶ���
			//�����mBoxMesh���л���֮ǰ��������任����
			//lamda���ʽ���Ҫ����ѧϰ
			mBoxMesh->mOnBeforeRenderCallback = [&](Renderer* render, Scene* scene, Camera* camera) {
				mBoxMesh->setPosition(camera->getWorldPosition());
				mBoxMesh->updateWorldMatrix();
			};

			//������Ҫ���������mesh����һ�ν�����������VBO��
			mObjects->update(mBoxMesh);
		}

		renderList->push(mBoxMesh, mBoxMesh->getGeometry(), mBoxMesh->getMaterial(), 0, 0);
	}

}