#pragma once
#include "../global/base.h"
#include "../global/constant.h"

namespace ff {

	//��������һ��ͼƬ�����Ľ��������
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
		//�����ͼƬ����
		std::vector<byte>	mData{};
		bool mNeedsUpdate{ true };

		//�п����ж��texture������ͬһ��source������source������һ���������,���������key-value��ʽ
		//����ÿһ��source��Ҫ���Լ���hashCode
		HashType mHashCode{ 0 };

		//�ڻ�������£�refCount��¼�˱�Source��ǰ�����ٸ��������ã�������Ϊ0��ж������
		uint32_t mRefCount{ 0 };
	};
}