#pragma once
#include "../global/base.h"

namespace ff {

	class Interpolant {
	public:
		using Ptr = std::shared_ptr<Interpolant>;

		Interpolant(
			const std::vector<float>& parameterPositions, 
			const std::vector<float>& sampleValues, 
			const uint32_t& sampleSize, 
			void* resultBuffer = nullptr) noexcept;

		~Interpolant() noexcept;

		//输入一个时间t，寻找在哪两个关键帧之间
		void evaluate(float t);

		void setBuffer(float* buffer);

	protected:
		void copySampleValue(const uint32_t& index);

		virtual void interpolateInternal(const uint32_t& lastIndex, const float& lastPosition, const float& nextPosition, const float& t) = 0;

	protected:
		//注意！这里是引用,说明interpolant并不真实存储数据，而是挂载到了外部的数据存储上
		const std::vector<float>&	mParameterPositions;//在动画系统中，保存的是关键帧的时间
		const std::vector<float>&	mSampleValues;//在动画系统中，保存的是关键帧的数据
		uint32_t					mSampleSize{ 0 };//关键帧数据的分割，itemSize。比如position就应该是3
		float*						mResultBuffer{ nullptr };//保存了每次插值运算的结果
		
		uint32_t					mCachedIndex{ 0 };//当前找到了哪一个关键帧
	};

	
}