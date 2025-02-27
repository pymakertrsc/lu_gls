#pragma once
#include "../global/base.h"
#include "../global/constant.h"
#include "keyframeTrack.h"

namespace ff {

	//对应着模型的一个动画，比如奔跑，比如出拳
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
		std::string	mName;//动画的名称
		float mTicksPerSecond{ 0.0f };
		float mDuration{ 0.0f };//本动作包含多少ticks
		std::vector<KeyframeTrack::Ptr> mTracks{};
	};
}