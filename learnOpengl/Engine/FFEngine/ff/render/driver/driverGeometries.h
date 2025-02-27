#pragma once
#include "../../global/base.h"
#include "../../core/geometry.h"
#include "driverAttributes.h"
#include "driverInfo.h"
#include "driverBindingState.h"

namespace ff {

	//�ڼ������ݲ����ϣ���backend�����ݽ��й���
	class DriverGeometries {
	public:
		using Ptr = std::shared_ptr<DriverGeometries>;
		static Ptr create(
			const DriverAttributes::Ptr& attributes,
			const DriverInfo::Ptr& info,
			const DriverBindingStates::Ptr& bindingStates
		) {
			return std::make_shared <DriverGeometries>(attributes, info, bindingStates);
		}

		DriverGeometries(
			const DriverAttributes::Ptr& attributes,
			const DriverInfo::Ptr& info,
			const DriverBindingStates::Ptr& bindingStates
		) noexcept;

		~DriverGeometries() noexcept;

		Geometry::Ptr get(const Geometry::Ptr& geometry) noexcept;

		void onGeometryDispose(const EventBase::Ptr& event);

		void update(const Geometry::Ptr& geometry) noexcept;
		
	private:
		DriverAttributes::Ptr mAttributes{ nullptr };
		DriverInfo::Ptr mInfo{ nullptr };

		//�����VAO�й�ϵ
		DriverBindingStates::Ptr mBindingStates{ nullptr };

		std::unordered_map<ID, bool> mGeometries{};
	};
}