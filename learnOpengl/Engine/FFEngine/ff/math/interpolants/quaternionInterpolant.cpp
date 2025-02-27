#include "quaternionInterpolant.h"

namespace ff {

	QuaternionInterpolant::QuaternionInterpolant(
		const std::vector<float>& parameterPositions,
		const std::vector<float>& sampleValues,
		const uint32_t& sampleSize,
		void* resultBuffer) noexcept :
		Interpolant(parameterPositions, sampleValues, sampleSize, resultBuffer) {}

	QuaternionInterpolant::~QuaternionInterpolant() noexcept {}

	void QuaternionInterpolant::interpolateInternal(
		const uint32_t& lastIndex,
		const float& lastPosition,
		const float& nextPosition,
		const float& t) {

		auto offsetLast = lastIndex * mSampleSize;
		auto offsetNext = offsetLast + mSampleSize;

		//因为使用了glm的库函数slerp做插值，所以得计算单独的比例
		auto weight = (t - lastPosition) / (nextPosition - lastPosition);

		//在模型当中读出来的四元数，在sampleValue里面的排列顺序为x y z w
		//在glm：：quat的构造函数的时候，w x y z
		glm::quat lastQuat = glm::quat(
			mSampleValues[offsetLast + 3],
			mSampleValues[offsetLast + 0],
			mSampleValues[offsetLast + 1],
			mSampleValues[offsetLast + 2]
		);

		glm::quat nextQuat = glm::quat(
			mSampleValues[offsetNext + 3],
			mSampleValues[offsetNext + 0],
			mSampleValues[offsetNext + 1],
			mSampleValues[offsetNext + 2]
		);

		glm::quat resultQuat = glm::slerp(lastQuat, nextQuat, weight);

		mResultBuffer[0] = resultQuat.x;
		mResultBuffer[1] = resultQuat.y;
		mResultBuffer[2] = resultQuat.z;
		mResultBuffer[3] = resultQuat.w;
	}
}