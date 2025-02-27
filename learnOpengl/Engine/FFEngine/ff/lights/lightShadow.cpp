#include "lightShadow.h"
#include "light.h"

namespace ff {
	LightShadow::LightShadow(const Camera::Ptr& camera) noexcept {
		mCamera = camera;
	}

	LightShadow::~LightShadow() noexcept {}

	void LightShadow::updateMatrices(const std::shared_ptr<Light>& light) noexcept {
		//将光源摄像机摆放到光源的位置
		mCamera->setLocalMatrix(light->getWorldMatrix());
		mCamera->updateWorldMatrix();

		//计算了projectionMatrix * viewMatrix
		auto pvMatrix = mCamera->getProjectionMatrix() * mCamera->getWorldMatrixInverse();
		mFrustum->setFromProjectionMatrix(pvMatrix);

		//to transpose one fragment's xyz to 0-1,but need colum-first!remember!
		//将x = （x + 1)/2，从-1到1，转化为0-1
		mMatrix = glm::mat4(
			0.5, 0.0, 0.0, 0.5,
			0.0, 0.5, 0.0, 0.5,
			0.0, 0.0, 0.5, 0.5,
			0.0, 0.0, 0.0, 1.0
		);

		//从行优先转为列优先存储
		mMatrix = glm::transpose(mMatrix) * pvMatrix;
	}

	uint32_t LightShadow::getViewportCount() const noexcept {
		return mViewports.size();
	}

	glm::vec4 LightShadow::getViewport(const uint32_t& index) {
		return mViewports[index];
	}

	Frustum::Ptr LightShadow::getFrustum() const noexcept {
		return mFrustum;
	}
}