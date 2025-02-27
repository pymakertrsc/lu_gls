#pragma once
#include "../global/base.h"
#include "../global/constant.h"
#include "keyframeTrack.h"

namespace ff {

	//��Ӧ��ģ�͵�һ�����������籼�ܣ������ȭ
	class AnimationClip {
	public:
		using Ptr = std::shared_ptr<AnimationClip>;
		static Ptr create(
			const std::string& name, 
			const float& ticksPerSecond,
			const float& duration, 
			const std::vector<KeyframeTrack::Ptr>& tracks) {
			return std::make_shared<AnimationClip>(name, ticksPerSecond, duration, tracks);
		}

		AnimationClip(
			const std::string& name, 
			const float& ticksPerSecond,
			const float& duration, 
			const std::vector<KeyframeTrack::Ptr>& tracks) noexcept;

		~AnimationClip() noexcept;

	public:
		std::string	mName;//����������
		float mTicksPerSecond{ 0.0f };
		float mDuration{ 0.0f };//��������������ticks
		std::vector<KeyframeTrack::Ptr> mTracks{};
	};
}