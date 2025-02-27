#pragma once
#include "../keyframeTrack.h"

namespace ff {

	class QuaternionKeyframeTrack :public KeyframeTrack {
	public:
		using Ptr = std::shared_ptr<QuaternionKeyframeTrack>;
		static Ptr create(
			const std::string& name,
			const std::vector<float>& values,
			const std::vector<float>& times) {
			return std::make_shared<QuaternionKeyframeTrack>(name, values, times);
		}

		QuaternionKeyframeTrack(
			const std::string& name,
			const std::vector<float>& values,
			const std::vector<float>& times) noexcept;

		~QuaternionKeyframeTrack() noexcept;

		Interpolant::Ptr makeInterpolant() noexcept override;
	};
}