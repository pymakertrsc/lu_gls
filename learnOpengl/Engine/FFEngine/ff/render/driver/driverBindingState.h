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

		//跟cache校验有关
		//存储了attribute的名字字符串作为key，attribute对象的id做为value
		std::unordered_map<std::string, ID> mAttributes{};

		//记录了对应的geometry的indexAttribute的id
		ID mIndex{ 0 };

		//记录了总共有多少个Attribute
		uint32_t mAttributesNum{ 0 };
	};

	//一个VAO与一个Geometry一一对应
	class DriverBindingStates {
	public:
		//key:geometry的ID号  value：BindingState这里面蕴含着一个VAO
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