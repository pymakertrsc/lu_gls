#pragma once
#include "../global/base.h"
#include "../global/constant.h"
#include "animationClip.h"
#include "../objects/skinnedMesh.h"
#include "propertyBinding.h"
#include "../math/interpolant.h"

namespace ff {

	//1 管理动画的开始 结束 速度
	//2 管理propertyBindings以及interpolants的更新
	class AnimationAction {
	public:
		using Ptr = std::shared_ptr<AnimationAction>;
		static Ptr create(const AnimationClip::Ptr& clip, const Object3D::Ptr& object) {
			return std::make_shared<AnimationAction>(clip, object);
		}

		AnimationAction(const AnimationClip::Ptr& clip, const Object3D::Ptr& object) noexcept;

		~AnimationAction() noexcept;

		void update(float deltaTime) noexcept;

		void play() noexcept;

		void stop() noexcept;

	public:
		std::string mName;//动画名称
		float mSpeed{ 1.0f };//播放速度
		bool mRunning{ false };//动画是否处于播放状态
		//by ticks
		float mCurrentTime{ 0.0f };//用ticks来计数的当前动画播放到了第几个ticks

		AnimationClip::Ptr	mClip{ nullptr };
		Object3D::Ptr		mRoot{ nullptr };//当前读取模型最后结果AssimpResult里面的mObject

		std::vector<PropertyBinding::Ptr>	mPropertyBindings{};//每个插值器对应的PropertyBindings
		std::vector<Interpolant::Ptr>		mInterpolants{};//当前mClip里面的每个KeyFrameTrack所对应的插值器集合
	};
}