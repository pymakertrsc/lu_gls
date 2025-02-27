#pragma once 
#include "../global/base.h"
#include "../global/constant.h"
#include "../core/object3D.h"

namespace ff {

	struct ParseNames {
		std::string mNodeName;//当前绑定的骨骼的名字
		std::string mPropertyName;//当前绑定的骨骼哪个属性"position" "rotation" "scale"
	};

	//每一个PropertyBinding都对应了一个KeyFrameTrack
	//存放了从rootObject开始搜寻，找到跟对应的KeyFrameTrack同样骨骼名称的Bone(Object3D)
	//存放了对应的当前 Bone，这个KeyFrameTrack到底影响的是哪个属性
	//通过apply函数，将每一帧interpolant计算后放入ResultBuffer当中的数据，更新到对应的bone的属性上
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
			const std::string& trackName,//比如：leftArm.position
			const size_t& valueSize//当前需要管理的属性，包括多少个float
		) noexcept;

		~PropertyBinding() noexcept;

		//将插值器运算得到的ResultBuffer更新到当前绑定的mNode（骨骼）的某个属性上去
		void apply() noexcept;

	private:
		Object3D::Ptr findeNode(const Object3D::Ptr& object, const std::string& nodeName) noexcept;

	public:
		Object3D::Ptr	mNode{ nullptr };
		ParseNames		mParseNames;

		void*			mBuffer{ nullptr };//对应的插值器的resultBuffer
	};
}