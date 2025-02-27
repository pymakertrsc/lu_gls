#include "animationClip.h"

namespace ff {

	AnimationClip::AnimationClip(
		const std::string& name,
		const float& ticksPerSecond,
		const float& duration,
		const std::vector<KeyframeTrack::Ptr>& tracks) noexcept {
		mName = name;
		mTicksPerSecond = ticksPerSecond;
		mDuration = duration;
		mTracks = tracks;
	}

	AnimationClip::~AnimationClip() noexcept {}
}