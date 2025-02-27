#pragma once 
#include "../global/base.h"
#include "../global/constant.h"
#include "../math/interpolant.h"

namespace ff {

	//某一个骨骼，某种变换的关键帧集合
	//比如，大腿骨的旋转关键帧的集合，小臂的平移关键帧集合
	class KeyframeTrack {
	public:
		using Ptr = std::shared_ptr<KeyframeTrack>;
		static Ptr create(
			const std::string& name,
			const std::vector<float>& values,
			const std::vector<float>& times) {
			return std::make_shared<KeyframeTrack>(name, values, times);
		}

		KeyframeTrack(
			const std::string& name,
			const std::vector<float>& values,
			const std::vector<float>& times) noexcept;

		~KeyframeTrack() noexcept;

		virtual Interpolant::Ptr makeInterpolant() noexcept;

		uint32_t getValueSize() noexcept;

	public:
		std::string			mName;//描述哪一个骨骼的哪一个关键帧集合，leftArm.rotation
		std::vector<float>	mValues;//对应着interpolant里面的sampleValue
		std::vector<float>	mTimes;//对应的interpolant里面的paramterPositions，关键帧的时间
	};
}