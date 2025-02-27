#pragma once
#include "../global/base.h"
#include "../core/object3D.h"

namespace ff {

	//worldMatrix:将顶点从模型坐标系，转换到世界坐标系
	//viewMatrix：将顶点从世界坐标系，转换到模型坐标系
	//逆矩阵（Inverse Matrix）A与B互为逆矩阵，那么A * B = I（单位阵）
	//Camera:一个是作为游戏主视角，作为光照的主视角
	class Camera:public Object3D {
	public:
		using Ptr = std::shared_ptr<Camera>;
	
		Camera() noexcept;

		~Camera() noexcept;

		glm::mat4 updateWorldMatrix(bool updateParent = false, bool updateChildren = false) noexcept override;

		glm::mat4 getWorldMatrixInverse() noexcept { return mWorldMatrixInverse; }

		glm::mat4 getProjectionMatrix() noexcept { return mProjectionMatrix; }

		virtual glm::mat4 updateProjectionMatrix() noexcept = 0;//纯虚函数,需要在子类当中实现

	protected:
		glm::mat4 mWorldMatrixInverse = glm::mat4(1.0f);
		glm::mat4 mProjectionMatrix = glm::mat4(1.0f);

	};
}