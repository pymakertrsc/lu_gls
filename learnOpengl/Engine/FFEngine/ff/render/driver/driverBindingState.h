#pragma once
#include "../../global/base.h"
#include "../../global/constant.h"
#include "../../core/geometry.h"
#include "../../core/object3D.h"
#include "../../core/attribute.h"
#include "../../material/material.h"
#include "driverAttributes.h"
#include "driverPrograms.h"

namespace ff {

	class DriverBindingStates;
	class DriverBindingState {
		friend DriverBindingStates;
	public:
		using Ptr = std::shared_ptr<DriverBindingState>;
		static Ptr create() {
			return std::make_shared <DriverBindingState>();
		}

		DriverBindingState() noexcept;

		~DriverBindingState() noexcept;

	private:
		GLuint	mVAO{ 0 };

		//��cacheУ���й�
		//�洢��attribute�������ַ�����Ϊkey��attribute�����id��Ϊvalue
		std::unordered_map<std::string, ID> mAttributes{};

		//��¼�˶�Ӧ��geometry��indexAttribute��id
		ID mIndex{ 0 };

		//��¼���ܹ��ж��ٸ�Attribute
		uint32_t mAttributesNum{ 0 };
	};

	//һ��VAO��һ��Geometryһһ��Ӧ
	class DriverBindingStates {
	public:
		//key:geometry��ID��  value��BindingState�������̺���һ��VAO
		using GeometryKeyMap = std::unordered_map<ID, DriverBindingState::Ptr>;

		using Ptr = std::shared_ptr<DriverBindingStates>;
		static Ptr create(const DriverAttributes::Ptr& attributes) {
			return std::make_shared <DriverBindingStates>(attributes);
		}

		DriverBindingStates(const DriverAttributes::Ptr& attributes);

		~DriverBindingStates();

		DriverBindingState::Ptr getBindingState(const Geometry::Ptr& geometry) noexcept;

		void setup(
			const Geometry::Ptr& geometry,
			const Attributei::Ptr& index) ;

		DriverBindingState::Ptr createBindingState(GLuint vao) noexcept;

		bool needsUpdate(const Geometry::Ptr& geometry, const Attributei::Ptr& index) noexcept;

		void saveCache(const Geometry::Ptr& geometry, const Attributei::Ptr& index) noexcept;

		void setupVertexAttributes(const Geometry::Ptr& geometry) noexcept;

		GLuint createVAO() noexcept;

		void bindVAO(GLuint vao) noexcept;

		void releaseStatesOfGeometry(ID geometryID) noexcept;

	private:
		DriverAttributes::Ptr	mAttributes{ nullptr };
		DriverBindingState::Ptr mCurrentBindingState{ nullptr };
		GeometryKeyMap	mBindingStates{};
	};
}