#include "quaternionKeyframeTrack.h"
#include "../../math/interpolants/quaternionInterpolant.h"

namespace ff {

	QuaternionKeyframeTrack::QuaternionKeyframeTrack(
		const std::string& name,
		const std::vector<float>& values,
		const std::vector<float>& times) noexcept :KeyframeTrack(name, values, times) {}

	QuaternionKeyframeTrack::~QuaternionKeyframeTrack() noexcept {}

	Interpolant::Ptr QuaternionKeyframeTrack::makeInterpolant() noexcept {
		return QuaternionInterpolant::create(mTimes, mValues, getValueSize());
	}
}