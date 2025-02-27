#pragma once
#include "../global/base.h"
#include "../global/constant.h"
#include "../textures/source.h"
#include "../global/eventDispatcher.h"

namespace ff {

	//���ж�ȡ���Ѿ��������source
	//���µ�source ���浽���������
	//source�ڱ������������ʹ��Ȩ��ʱ���ܹ�֪��������û�����õ�ʱ������
	class Cache {
	public:
		static Cache* getInstance();

		Cache() noexcept;

		~Cache() noexcept;

		Source::Ptr getSource(const std::string& path) noexcept;

		void cacheSource(const std::string& path, Source::Ptr source) noexcept;

		void onSourceRelease(const EventBase::Ptr& e);

	private:
		static Cache* mInstance;

		//hashType = size_t
		std::unordered_map<HashType, Source::Ptr> mSources{};
	};
}