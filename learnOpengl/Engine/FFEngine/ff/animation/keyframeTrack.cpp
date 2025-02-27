#pragma once 
#include "keyframeTrack.h"
#include "../math/interpolants/linearInterpolant.h"

namespace ff {

	KeyframeTrack::KeyframeTrack(
		const std::string& name,
		const std::vector<float>& values,
		const std::vector<float>& times) noexcept {
		mName = name;
		mValues = values;
		mTimes = times;
	}

	KeyframeTrack::~KeyframeTrack() noexcept {}

	//������Ӧ��KeyFrameTrack��interpolant��ֵ��
	Interpolant::Ptr KeyframeTrack::makeInterpolant() noexcept {
		return LinearInterpolant::create(mTimes, mValues, getValueSize());
	}

	//��ȡ��ӦInterpolant��sampleSize������position�任������3��������Ԫ���任������4
	uint32_t KeyframeTrack::getValueSize() noexcept {
		return mValues.size() / mTimes.size();
	}

}