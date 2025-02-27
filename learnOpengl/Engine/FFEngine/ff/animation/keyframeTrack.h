#pragma once 
#include "../global/base.h"
#include "../global/constant.h"
#include "../math/interpolant.h"

namespace ff {

	//ĳһ��������ĳ�ֱ任�Ĺؼ�֡����
	//���磬���ȹǵ���ת�ؼ�֡�ļ��ϣ�С�۵�ƽ�ƹؼ�֡����
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
		std::string			mName;//������һ����������һ���ؼ�֡���ϣ�leftArm.rotation
		std::vector<float>	mValues;//��Ӧ��interpolant�����sampleValue
		std::vector<float>	mTimes;//��Ӧ��interpolant�����paramterPositions���ؼ�֡��ʱ��
	};
}