#pragma once
#include "../keyframeTrack.h"

namespace ff {

	class VectorKeyframeTrack :public KeyframeTrack {
	public:
		using Ptr = std::shared_ptr<VectorKeyframeTrack>;
		static Ptr create(
			const std::string& name,
			const std::vector<float>& values,
			const std::vector<float>& times) {
			return std::make_shared<VectorKeyframeTrack>(name, values, times);
		}

		VectorKeyframeTrack(
			const std::string& name,
			const std::vector<float>& values,
			const std::vector<float>& times) noexcept;

		~VectorKeyframeTrack() noexcept;
	};
}