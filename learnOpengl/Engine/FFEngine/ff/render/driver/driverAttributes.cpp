#include "driverAttributes.h"

namespace ff {

		DriverAttribute::DriverAttribute() noexcept {}

		DriverAttribute::~DriverAttribute() noexcept {
			if (mHandle) {
				glDeleteBuffers(1, &mHandle);
			}
		}

		DriverAttributes::DriverAttributes() noexcept {
			EventDispatcher::getInstance()->addEventListener("attributeDispose", this, &DriverAttributes::onAttributeDispose);
		}

		DriverAttributes::~DriverAttributes() noexcept {
			EventDispatcher::getInstance()->removeEventListener("attributeDispose", this, &DriverAttributes::onAttributeDispose);
		}

		void DriverAttributes::remove(ID attributeID) noexcept {
			auto iter = mAttributes.find(attributeID);
			if (iter != mAttributes.end()) {
				mAttributes.erase(iter);
			}
		}

		void DriverAttributes::onAttributeDispose(const EventBase::Ptr& e) {
			ID attrID = *((ID*)e->mpUserData);
			remove(attrID);
		}
}