#pragma once 
#include "../global/base.h"
#include "../global/constant.h"
#include "../core/object3D.h"

namespace ff {

	struct ParseNames {
		std::string mNodeName;//��ǰ�󶨵Ĺ���������
		std::string mPropertyName;//��ǰ�󶨵Ĺ����ĸ�����"position" "rotation" "scale"
	};

	//ÿһ��PropertyBinding����Ӧ��һ��KeyFrameTrack
	//����˴�rootObject��ʼ��Ѱ���ҵ�����Ӧ��KeyFrameTrackͬ���������Ƶ�Bone(Object3D)
	//����˶�Ӧ�ĵ�ǰ Bone�����KeyFrameTrack����Ӱ������ĸ�����
	//ͨ��apply��������ÿһ֡interpolant��������ResultBuffer���е����ݣ����µ���Ӧ��bone��������
	class PropertyBinding {
	public:
		using Ptr = std::shared_ptr<PropertyBinding>;
		static Ptr create(
			const Object3D::Ptr& root,
			const std::string& trackName,
			const size_t& valueSize
		) {
			return std::make_shared<PropertyBinding>(root, trackName, valueSize);
		}

		PropertyBinding(
			const Object3D::Ptr& root,
			const std::string& trackName,//���磺leftArm.position
			const size_t& valueSize//��ǰ��Ҫ��������ԣ��������ٸ�float
		) noexcept;

		~PropertyBinding() noexcept;

		//����ֵ������õ���ResultBuffer���µ���ǰ�󶨵�mNode����������ĳ��������ȥ
		void apply() noexcept;

	private:
		Object3D::Ptr findeNode(const Object3D::Ptr& object, const std::string& nodeName) noexcept;

	public:
		Object3D::Ptr	mNode{ nullptr };
		ParseNames		mParseNames;

		void*			mBuffer{ nullptr };//��Ӧ�Ĳ�ֵ����resultBuffer
	};
}