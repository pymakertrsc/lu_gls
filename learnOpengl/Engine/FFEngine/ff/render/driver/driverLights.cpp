#include "driverLights.h"
#include "../../lights/directionalLight.h"

namespace ff {

	DriverLights::DriverLights() noexcept {
		mState.mLightUniformHandles["ambientLightColor"] = UniformHandle();

		UniformHandle directionalLights;
		directionalLights.mValue = UniformUnitMap();

		UniformHandle directionalLightShadows;
		directionalLightShadows.mValue = UniformUnitMap();

		UniformHandle directionalShadowMap;
		directionalShadowMap.mValue = std::vector<Texture::Ptr>();

		UniformHandle directionalShadowMatrix;
		directionalShadowMatrix.mValue = std::vector<glm::mat4>();

		mState.mLightUniformHandles["directionalLights"] = directionalLights;
		mState.mLightUniformHandles["directionalLightShadows"] = directionalLightShadows;
		mState.mLightUniformHandles["directionalShadowMap"] = directionalShadowMap;
		mState.mLightUniformHandles["directionalShadowMatrix"] = directionalShadowMatrix;
	}

	DriverLights::~DriverLights() noexcept {}

	void DriverLights::init() noexcept {
	}

	void DriverLights::setupLights(std::vector<Light::Ptr>& lights) noexcept {

		uint32_t directionalLightCount = 0;
		uint32_t numDirectionalShadows = 0;

		float r = 0, g = 0, b = 0;

		//to make shadowLight in front of others for shader
		std::sort(lights.begin(), lights.end(), shadowCastingLightsFirst);

		//prepare shadow uniforms
		auto& lightArray = mState.mLightUniformHandles["directionalLights"];

		//UniformHandle
		auto& shadowStructuredArray = mState.mLightUniformHandles["directionalLightShadows"];
		auto& shadowMapPureArray = mState.mLightUniformHandles["directionalShadowMap"];
		auto& shadowMatrixPureArray = mState.mLightUniformHandles["directionalShadowMatrix"];

		lightArray.mNeedsUpdate = true;
		shadowStructuredArray.mNeedsUpdate = true;
		shadowMapPureArray.mNeedsUpdate = true;
		shadowMatrixPureArray.mNeedsUpdate = true;

		clearPureArrayUniform(std::any_cast<std::vector<Texture::Ptr>>(&shadowMapPureArray.mValue));
		clearPureArrayUniform(std::any_cast<std::vector<glm::mat4>>(&shadowMatrixPureArray.mValue));

		for (const Light::Ptr& light : lights) {
			auto color = light->mColor;
			auto intensity = light->mIntensity;

			Texture::Ptr shadowMap = (light->mShadow && light->mShadow->mRenderTarget) ? light->mShadow->mRenderTarget->getTexture() : nullptr;

			//maybe multi-ambient
			//在这里将所有ambientLight的影响都累加
			if (light->mIsAmbientLight) {
				r += color.r * intensity;
				g += color.g * intensity;
				b += color.b * intensity;
			}
			else if (light->mIsDirectionalLight) {

				//add one directionalLight
				UniformUnitMap* directionalLightUniform = setArrayStructuredUniform(directionalLightCount, std::any_cast<UniformUnitMap>(&lightArray.mValue));

				(*directionalLightUniform)["color"] = light->mColor * light->mIntensity;

				if (light->mCastShadow) {
					LightShadow::Ptr shadow = light->mShadow; 

					//shadow uniform
					UniformUnitMap* directionalShadowUniform = setArrayStructuredUniform(directionalLightCount, std::any_cast<UniformUnitMap>(&shadowStructuredArray.mValue));
					(*directionalShadowUniform)["shadowBias"] = shadow->mBias;
					(*directionalShadowUniform)["shadowRadius"] = shadow->mRadius;
					(*directionalShadowUniform)["shadowMapSize"] = shadow->mMapSize;

					//matrix and shadowmap will update when rendering shadow map

					numDirectionalShadows++;
				}
				directionalLightCount++;
			}
		}
		mState.mDirectionalCount = directionalLightCount;
		mState.mNumDirectionalShadows = numDirectionalShadows;

		mState.mLightUniformHandles["ambientLightColor"].mValue = glm::vec3(r, g, b);

		if (
			mState.mCache.mDirectionalCount != mState.mDirectionalCount ||
			mState.mCache.mNumDirectionalShadows != mState.mNumDirectionalShadows
			) {
			mState.mCache.mDirectionalCount = mState.mDirectionalCount;
			mState.mCache.mNumDirectionalShadows = mState.mNumDirectionalShadows;

			mState.mVersion++;
		}
	}

	void DriverLights::setupLightsView(const std::vector<Light::Ptr>& lights, const Camera::Ptr& camera) noexcept {
		uint32_t directionalLength = 0;
		auto viewMatrix = camera->getWorldMatrixInverse();


		auto& lightArray = mState.mLightUniformHandles["directionalLights"];
		for (uint32_t i = 0; i < lights.size(); ++i) {
			auto light = lights[i];

			if (light->mIsDirectionalLight) {
				auto dLight = std::static_pointer_cast<DirectionalLight>(light);
				UniformUnitMap* uniform = getArrayStructuredUniform(directionalLength, std::any_cast<UniformUnitMap>(&lightArray.mValue));

				auto lightDirection = light->getWorldDirection();
				auto lightViewDirection = glm::mat3(viewMatrix) * lightDirection;
				(*uniform)["direction"] = lightViewDirection;

				directionalLength++;
			}
		}
	}

}