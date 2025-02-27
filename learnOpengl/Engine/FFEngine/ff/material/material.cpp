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

		//再析构的时候，向外发出消息，通知监听函数们，哪一个材质消亡了
		EventBase::Ptr e = EventBase::create("materialDispose");
		e->mTarget = this;
		EventDispatcher::getInstance()->dispatchEvent(e);
	}

}