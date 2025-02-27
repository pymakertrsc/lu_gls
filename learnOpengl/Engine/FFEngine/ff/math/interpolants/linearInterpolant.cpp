#include "linearInterpolant.h"

namespace ff {

	LinearInterpolant::LinearInterpolant(
		const std::vector<float>& parameterPositions,
		const std::vector<float>& sampleValues,
		const uint32_t& sampleSize,
		void* resultBuffer) noexcept :
		Interpolant(parameterPositions, sampleValues, sampleSize, resultBuffer) {}

	LinearInterpolant::~LinearInterpolant() noexcept {}

	void LinearInterpolant::interpolateInternal(
		const uint32_t& lastIndex,
		const float& lastPosition,
		const float& nextPosition,
		const float& t) {

		auto offsetLast = lastIndex * mSampleSize;
		auto offsetNext = offsetLast + mSampleSize;

		auto weightNext = (t - lastPosition) / (nextPosition - lastPosition);
		auto weightLast = 1.0f - weightNext;

		//假设是位置信息，则对xyz分别做了加权相加
		for (uint32_t i = 0; i < mSampleSize; ++i) {
			mResultBuffer[i] = mSampleValues[offsetLast + i] * weightLast + mSampleValues[offsetNext + i] * weightNext;
		}
	}
}