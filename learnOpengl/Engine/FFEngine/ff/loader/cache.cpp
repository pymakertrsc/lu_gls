#include "cache.h"
#include "../textures/texture.h"
#include "../textures/cubeTexture.h"

namespace ff {
	Cache* Cache::mInstance = nullptr;
	Cache* Cache::getInstance() {
		if (mInstance == nullptr) {
			mInstance = new Cache();
		}

		return mInstance;
	}

	Cache::Cache() noexcept {
		EventDispatcher::getInstance()->addEventListener("sourceRelease", this, &Cache::onSourceRelease);
	}

	Cache::~Cache() noexcept {}

	Source::Ptr Cache::getSource(const std::string& path) noexcept {
		std::hash<std::string> hasher;
		auto hashCode = hasher(path);

		Source::Ptr source = nullptr;

		auto iter = mSources.find(hashCode);
		if (iter != mSources.end()) {
			source = iter->second;

			//每次调用get，必然有一个texture在使用他，refCount +1
			source->mRefCount++;
		}

		return source;
	}

	//path可能是硬盘文件路径，也可能是网络数据流的url，也可能是嵌入式纹理在模型当中的path/name
	void Cache::cacheSource(const std::string& path, Source::Ptr source) noexcept {
		std::hash<std::string> hasher;
		auto hashCode = hasher(path);

		//寻找是否重复
		auto iter = mSources.find(hashCode);
		if (iter != mSources.end()) {
			return;
		}

		//没找到同名同姓的source
		source->mHashCode = hashCode;

		//每次只要生成source，就一定会有一个texture来使用他，refCount就必然+1
		//Todo：我们可以将refCount做成private，friend Cache，就会造成只能在cache里访问
		source->mRefCount++;

		mSources.insert(std::make_pair(hashCode, source));
	}

	//cache会监听sourceRelease
	void Cache::onSourceRelease(const EventBase::Ptr& e) {
		auto source = static_cast<Source*>(e->mTarget);
		auto hashCode = source->mHashCode;

		auto iter = mSources.find(hashCode);
		if (iter == mSources.end()) {
			return;
		}

		//如果确实存在在cache里面，则引用计数-1
		source->mRefCount--;

		//如果引用计数 = 0，说明整个程序没有人再使用这个source，那么就从cache里清除
		if (source->mRefCount == 0) {

			//source通过sharedPtr管理，那么再erase之后，并且以前持有他的对象都不再持有，
			//source这个智能指针的引用计数变为了0，则会构成source的析构
			mSources.erase(iter);
		}
	}
}