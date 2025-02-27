#include "material.h"
#include "../tools/identity.h"
#include "../global/eventDispatcher.h"

namespace ff {

	Material::Material() noexcept {
		mID = Identity::generateID();
		mType = MaterialName::Material;
		mIsMaterial = true;
	}

	Material::~Material() noexcept {

		//��������ʱ�����ⷢ����Ϣ��֪ͨ���������ǣ���һ������������
		EventBase::Ptr e = EventBase::create("materialDispose");
		e->mTarget = this;
		EventDispatcher::getInstance()->dispatchEvent(e);
	}

}