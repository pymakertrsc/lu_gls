#pragma once
#include "../global/base.h"
#include "../global/constant.h"
#include "animationClip.h"
#include "../objects/skinnedMesh.h"
#include "propertyBinding.h"
#include "../math/interpolant.h"

namespace ff {

	//1 �������Ŀ�ʼ ���� �ٶ�
	//2 ����propertyBindings�Լ�interpolants�ĸ���
	class AnimationAction {
	public:
		using Ptr = std::shared_ptr<AnimationAction>;
		static Ptr create(const AnimationClip::Ptr& clip, const Object3D::Ptr& object) {
			return std::make_shared<AnimationAction>(clip, object);
		}

		AnimationAction(const AnimationClip::Ptr& clip, const Object3D::Ptr& object) noexcept;

		~AnimationAction() noexcept;

		void update(float deltaTime) noexcept;

		void play() noexcept;

		void stop() noexcept;

	public:
		std::string mName;//��������
		float mSpeed{ 1.0f };//�����ٶ�
		bool mRunning{ false };//�����Ƿ��ڲ���״̬
		//by ticks
		float mCurrentTime{ 0.0f };//��ticks�������ĵ�ǰ�������ŵ��˵ڼ���ticks

		AnimationClip::Ptr	mClip{ nullptr };
		Object3D::Ptr		mRoot{ nullptr };//��ǰ��ȡģ�������AssimpResult�����mObject

		std::vector<PropertyBinding::Ptr>	mPropertyBindings{};//ÿ����ֵ����Ӧ��PropertyBindings
		std::vector<Interpolant::Ptr>		mInterpolants{};//��ǰmClip�����ÿ��KeyFrameTrack����Ӧ�Ĳ�ֵ������
	};
}