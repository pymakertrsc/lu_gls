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

		//��Ϊʹ����glm�Ŀ⺯��slerp����ֵ�����Եü��㵥���ı���
		auto weight = (t - lastPosition) / (nextPosition - lastPosition);

		//��ģ�͵��ж���������Ԫ������sampleValue���������˳��Ϊx y z w
		//��glm����quat�Ĺ��캯����ʱ��w x y z
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