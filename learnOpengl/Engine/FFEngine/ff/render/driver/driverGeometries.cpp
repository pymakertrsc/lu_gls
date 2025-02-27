#include "driverGeometries.h"
#include "../../global/eventDispatcher.h"

namespace ff {

	DriverGeometries::DriverGeometries(
		const DriverAttributes::Ptr& attributes,
		const DriverInfo::Ptr& info,
		const DriverBindingStates::Ptr& bindingStates
	) noexcept {
		mAttributes = attributes;
		mInfo = info;
		mBindingStates = bindingStates;

		EventDispatcher::getInstance()->addEventListener("geometryDispose", this, &DriverGeometries::onGeometryDispose);
	}

	DriverGeometries::~DriverGeometries() noexcept {
		EventDispatcher::getInstance()->removeEventListener("geometryDispose", this, &DriverGeometries::onGeometryDispose);
	}

	void DriverGeometries::onGeometryDispose(const EventBase::Ptr& event) {
		const auto geometry = static_cast<Geometry*>(event->mTarget);

		mGeometries.erase(geometry->getID());

		//因为一个geometry会对应一个driverBindingState，即一个vao
		mBindingStates->releaseStatesOfGeometry(geometry->getID());

		mInfo->mMemory.mGeometries--;
	}

	//给了一个机会， 可以在每一次update的之前，对geometry相关数据做一次更新
	Geometry::Ptr DriverGeometries::get(const Geometry::Ptr& geometry) noexcept {
//geometries是一个id-bool类型的map，用来记录当前这个geometry是否被计算过info一次

		auto iter = mGeometries.find(geometry->getID());
		if (iter != mGeometries.end()) {
			if (iter->second == true) {
				return geometry;
			}
		}

		mGeometries[geometry->getID()] = true;

		mInfo->mMemory.mGeometries++;

		return geometry;
	}

	void DriverGeometries::update(const Geometry::Ptr& geometry) noexcept {
		const auto geometryAttributes = geometry->getAttributes();
		for (const auto& iter: geometryAttributes) {
			//只处理除了indexAttribute之外的attributes 
			mAttributes->update(iter.second , BufferType::ArrayBuffer);
		}
	}
}