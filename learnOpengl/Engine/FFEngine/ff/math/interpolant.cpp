#include "interpolant.h"

namespace ff {

	Interpolant::Interpolant(
		const std::vector<float>& parameterPositions, 
		const std::vector<float>& sampleValues, 
		const uint32_t& sampleSize, 
		void* resultBuffer) noexcept :
		mParameterPositions(parameterPositions), mSampleValues(sampleValues) {
		mSampleSize = sampleSize;
		mResultBuffer = static_cast<float*>(resultBuffer);
	}

	Interpolant::~Interpolant() noexcept {}

	//t是对动画总时间长度取了余数的结果
	void Interpolant::evaluate(float t) {
		const auto& pp = mParameterPositions;

		//对于当前时间t，一定位于两个关键帧之间（闭区间）
		//lastIndex:前一个关键帧
		//nextIndex:后一个关键帧
		uint32_t	lastIndex = 0;

		//mCachedIndex里面存储了上一次插值的时候的nextIndex
		if (mCachedIndex != 0) {
			lastIndex = mCachedIndex - 1;
		}

		if (pp.size() == 1) {
			//just copy the only one
			copySampleValue(0);
			return;
		}
		else {
			//check the bound
			if (t < pp[0]) {
				//smaller than the first one, just give out the smallest
				copySampleValue(0);
				return;
			}

			if (t > pp[pp.size() - 1]) {
				//bigger than the last one, just give out the biggest
				copySampleValue(pp.size() - 1);
				return;
			}

			//check if back to the beginning or just go forward
			if (t < pp[lastIndex]) {
				//check from begin
				lastIndex = 0;
			}

			//go forward to determin lastIndex
			while (t > pp[lastIndex + 1]) {
				lastIndex++;
			}
		}

		mCachedIndex = lastIndex + 1;
		interpolateInternal(lastIndex, mParameterPositions[lastIndex], mParameterPositions[lastIndex + 1], t);
	}

	void Interpolant::setBuffer(float* buffer) {
		mResultBuffer = buffer;
	}

	//给定一个关键帧的索引Index，将mResultBuffer设置为这个关键帧所对应的sampleValue
	void Interpolant::copySampleValue(const uint32_t& index) {
		if (index > mParameterPositions.size() - 1) {
			std::cout << "Error: index is bigger than bound when copySampleValue" << std::endl;
			return;
		}

		assert(mResultBuffer);

		for (uint32_t i = 0; i < mSampleSize; ++i) {
			mResultBuffer[i] = mSampleValues[index * mSampleSize + i];
		}
	}
}