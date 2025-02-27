#pragma once
#include"../../global/base.h"
#include "../../global/constant.h"
#include "../../lights/light.h"
#include "../../global/eventDispatcher.h"
#include "../../camera/camera.h"
#include "../shaders/uniformsLib.h"

namespace ff {

	//>��ʾ�����ǰ��
	static bool  shadowCastingLightsFirst(const Light::Ptr& light0, const Light::Ptr& light1) {
		int flag0 = light0->mCastShadow ? 1 : 0;
		int flag1 = light1->mCastShadow ? 1 : 0;
		return flag0 > flag1;
	}

	//����
	// 1 �洢/���¸������йص�UniformHandleMap
	// 2 ��Shader�Ƿ���£��ṩ���� (�����Դ�����������Դ������Ӱ��������
	//
	class DriverLights {
	public:
		struct State {

			//��һ֡���������
			struct Cache {
				uint32_t mDirectionalCount = 0;
				uint32_t mNumDirectionalShadows = 0;
			};

			//��ǰƽ�й����������ǰ������Ӱ��ƽ�й�����
			uint32_t mDirectionalCount = 0;
			uint32_t mNumDirectionalShadows = 0;

			UniformHandleMap mLightUniformHandles{};

			//ÿ��ֻҪ���֣����������ͬ��mVersion�ͻ�+1
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