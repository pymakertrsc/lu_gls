#include "animationAction.h"

namespace ff {

	AnimationAction::AnimationAction(const AnimationClip::Ptr& clip, const Object3D::Ptr& object) noexcept {
		mName = clip->mName;
		mClip = clip;
		mRoot = object;

		auto tracks = clip->mTracks;
		auto nTracks = tracks.size();

		//mInterpolants����ÿһ����ֵ������Ӧ��һ��KeyFrameTrack
		//mPropertyBindings���е�ÿһ������Ӧ�洢һ��Interpolant����ֵ������ResultBuffer
		//mPropertyBindings���е�ÿһ������Ӧ�ŵ�ǰ������һ�����������ԣ�Ҫô��position Ҫô��rotation Ҫô��scale��
		mInterpolants.resize(nTracks);
		mPropertyBindings.resize(nTracks);

		//make interpolants
		for (uint32_t i = 0; i < nTracks; ++i) {
			mInterpolants[i] = tracks[i]->makeInterpolant();
		}

		//make bindings 
		for (uint32_t i = 0; i < nTracks; ++i) {
			mPropertyBindings[i] = PropertyBinding::create(mRoot, tracks[i]->mName, tracks[i]->getValueSize());
			mInterpolants[i]->setBuffer(static_cast<float*>(mPropertyBindings[i]->mBuffer));
		}
	}

	AnimationAction::~AnimationAction() noexcept {}

	void AnimationAction::play() noexcept {
		mRunning = true;
	}

	void AnimationAction::stop() noexcept {
		mRunning = false;
	}

	void AnimationAction::update(float deltaTime) noexcept {
		if (!mRunning) {
			return;
		}

		float duration = mClip->mDuration;
		float ticksPerSecond = mClip->mTicksPerSecond;

		mCurrentTime = fmod(mCurrentTime + deltaTime * ticksPerSecond * mSpeed, duration);

		//�Ե�ǰ���е�keyFrameTracks��һ�β�ֵ���㣬����Ľ�������ڶ�Ӧ��PropertyBinding��mBuffer����
		for (uint32_t i = 0; i < mInterpolants.size(); ++i) {
			mInterpolants[i]->evaluate(mCurrentTime);
		}

		//��ʱÿһ��PropertyBinding�����mBuffer���Ѿ�����˵�ǰʱ�����µĲ�ֵ��������position��quternion��scale��
		//ѭ������ÿһ��PropertyBinding��ʹ��apply��������������Ӧ��mNode��Bone���Ķ�Ӧ���ԣ�ʹ�����µ�mBuffer���и���
		for (uint32_t i = 0; i < mPropertyBindings.size(); ++i) {
			mPropertyBindings[i]->apply();
		}
	}
}