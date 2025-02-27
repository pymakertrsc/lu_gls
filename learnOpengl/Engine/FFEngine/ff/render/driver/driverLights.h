#pragma once
#include"../../global/base.h"
#include "../../global/constant.h"
#include "../../lights/light.h"
#include "../../global/eventDispatcher.h"
#include "../../camera/camera.h"
#include "../shaders/uniformsLib.h"

namespace ff {

	//>表示大的在前面
	static bool  shadowCastingLightsFirst(const Light::Ptr& light0, const Light::Ptr& light1) {
		int flag0 = light0->mCastShadow ? 1 : 0;
		int flag1 = light1->mCastShadow ? 1 : 0;
		return flag0 > flag1;
	}

	//任务：
	// 1 存储/更新跟光照有关的UniformHandleMap
	// 2 对Shader是否更新，提供依据 (各类光源数量，各类光源产生阴影的数量）
	//
	class DriverLights {
	public:
		struct State {

			//上一帧的数量情况
			struct Cache {
				uint32_t mDirectionalCount = 0;
				uint32_t mNumDirectionalShadows = 0;
			};

			//当前平行光的数量，当前产生阴影的平行光数量
			uint32_t mDirectionalCount = 0;
			uint32_t mNumDirectionalShadows = 0;

			UniformHandleMap mLightUniformHandles{};

			//每次只要发现，相关数量不同，mVersion就会+1
			uint32_t mVersion{ 1 };

			Cache mCache{};
		};

		using Ptr = std::shared_ptr<DriverLights>;
		static Ptr create() {
			return std::make_shared<DriverLights>();
		}

		DriverLights() noexcept;

		~DriverLights() noexcept;

		void	 init() noexcept;

		void setupLights(std::vector<Light::Ptr>& lights) noexcept;

		void setupLightsView(const std::vector<Light::Ptr>& lights, const Camera::Ptr& camera) noexcept;

	public:
		State mState;
	};
}