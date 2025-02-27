#pragma once
#include "../global/base.h"
#include "../global/constant.h"

namespace ff {

	//用来管理一张图片读入后的结果即数据
	class Source {
	public:
		using Ptr = std::shared_ptr<Source>;
		static Ptr create() {
			return std::make_shared <Source>();
		}

		Source() noexcept;

		~Source() noexcept;

	public:
		uint32_t			mWidth{ 0 };
		uint32_t			mHeight{ 0 };
		//读入的图片数据
		std::vector<byte>	mData{};
		bool mNeedsUpdate{ true };

		//有可能有多个texture引用了同一个source，所以source必须有一个缓存机制,缓存必须是key-value形式
		//所以每一个source都要有自己的hashCode
		HashType mHashCode{ 0 };

		//在缓存情况下，refCount记录了本Source当前被多少个对象引用，当引用为0，卸载析构
		uint32_t mRefCount{ 0 };
	};
}