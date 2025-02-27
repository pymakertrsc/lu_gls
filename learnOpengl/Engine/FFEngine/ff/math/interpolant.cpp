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

	//t�ǶԶ�����ʱ�䳤��ȡ�������Ľ��
	void Interpolant::evaluate(float t) {
		const auto& pp = mParameterPositions;

		//���ڵ�ǰʱ��t��һ��λ�������ؼ�֮֡�䣨�����䣩
		//lastIndex:ǰһ���ؼ�֡
		//nextIndex:��һ���ؼ�֡
		uint32_t	lastIndex = 0;

		//mCachedIndex����洢����һ�β�ֵ��ʱ���nextIndex
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

	//����һ���ؼ�֡������Index����mResultBuffer����Ϊ����ؼ�֡����Ӧ��sampleValue
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