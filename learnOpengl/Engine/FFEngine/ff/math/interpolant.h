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

		//����һ��ʱ��t��Ѱ�����������ؼ�֮֡��
		void evaluate(float t);

		void setBuffer(float* buffer);

	protected:
		void copySampleValue(const uint32_t& index);

		virtual void interpolateInternal(const uint32_t& lastIndex, const float& lastPosition, const float& nextPosition, const float& t) = 0;

	protected:
		//ע�⣡����������,˵��interpolant������ʵ�洢���ݣ����ǹ��ص����ⲿ�����ݴ洢��
		const std::vector<float>&	mParameterPositions;//�ڶ���ϵͳ�У�������ǹؼ�֡��ʱ��
		const std::vector<float>&	mSampleValues;//�ڶ���ϵͳ�У�������ǹؼ�֡������
		uint32_t					mSampleSize{ 0 };//�ؼ�֡���ݵķָitemSize������position��Ӧ����3
		float*						mResultBuffer{ nullptr };//������ÿ�β�ֵ����Ľ��
		
		uint32_t					mCachedIndex{ 0 };//��ǰ�ҵ�����һ���ؼ�֡
	};

	
}