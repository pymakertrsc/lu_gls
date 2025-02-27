#pragma once
#include "../../global/base.h"
#include "../../core/attribute.h"
#include "../../global/eventDispatcher.h"

namespace ff {

	//DriverAttribute 是与Attribute一一对应
	class DriverAttribute {
	public:
		using Ptr = std::shared_ptr<DriverAttribute>;
		static Ptr create() {
			return std::make_shared <DriverAttribute>();
		}

		DriverAttribute() noexcept;

		~DriverAttribute() noexcept;

		//mHandle就是VBO
		//VBO，是OpenGL的一个概念，VertexBufferObject。代表了在GPU 上的一块内存空间，专门用来
		//存储Mesh顶点的Attribute数据
		GLuint		mHandle{ 0 };
	};

	class DriverAttributes {
	public:
		using Ptr = std::shared_ptr<DriverAttributes>;
		static Ptr create() {
			return std::make_shared <DriverAttributes>();
		}

		//ID:前端的Attribute的ID
		//Value：本ID所对应的Attribute生成的DriverAttribute
		using DriverAttributesMap = std::unordered_map<ID, DriverAttribute::Ptr>;
		DriverAttributes() noexcept;

		~DriverAttributes() noexcept;

		//功能：
		// 1 如果本Attribute没有对应的DriverAttribute，就为其生成，且更新数据
		// 2 如果本Attribute拥有一个对应的DriverAttribute，则检查其VBO 数据是否需要更新
		//bufferType表示了本Attribute是一个indexAttribute或者是一个普通的Attribute
		template<typename T>
		DriverAttribute::Ptr update(
			const std::shared_ptr<Attribute<T>>& attribute,
			const BufferType& bufferType) noexcept;

		template<typename T>
		DriverAttribute::Ptr get(const std::shared_ptr<Attribute<T>>& attribute) noexcept;

		void remove(ID attributeID) noexcept;

		void onAttributeDispose(const EventBase::Ptr& e);

	private:
		DriverAttributesMap mAttributes{};
	};

	template<typename T>
	DriverAttribute::Ptr DriverAttributes::get(const std::shared_ptr<Attribute<T>>& attribute) noexcept {
		auto iter = mAttributes.find(attribute->getID());
		if (iter != mAttributes.end()) {
			return iter->second;
		}

		return nullptr;
	}

	template<typename T>
	DriverAttribute::Ptr DriverAttributes::update(
		const std::shared_ptr<Attribute<T>>& attribute,
		const BufferType& bufferType
	) noexcept {
		DriverAttribute::Ptr dattribute = nullptr;

		//寻找，看看是否我们的map里有本Attribute对应的DriverAttribute
		auto iter = mAttributes.find(attribute->getID());
		if (iter != mAttributes.end()) {
			dattribute = iter->second;
		}
		else {

			//如果没有找到，则创建一个新的DriverAttribute
			dattribute = DriverAttribute::create();

			//拿到Attribute里面的数据
			auto data = attribute->getData();

			//为本Attribute对应的DriverAttribute生成VBO 并且更新数据
			glGenBuffers(1, &dattribute->mHandle);

			//bufferType要么是GL_ARRAY_BUFFER 要么是 GL_ELEMENT_ARRAY_BUFFER
			glBindBuffer(toGL(bufferType), dattribute->mHandle);

			//VBO内存开辟，以及VBO 数据的灌入
			glBufferData(toGL(bufferType), data.size() * sizeof(T), data.data(), toGL(attribute->getBufferAllocType()));
			glBindBuffer(toGL(bufferType), 0);

			mAttributes.insert(std::make_pair(attribute->getID(), dattribute));

			//将Attribute的需要更新的状态清空
			attribute->clearUpdateRange();
			attribute->clearNeedsUpdate();
		}

		//如果原来就存在DriverAttribute，那么就得检查是否需要更新
		if (attribute->getNeedsUpdate()) {
			attribute->clearNeedsUpdate();

			//获取更新的offset以及Count
			auto updateRange = attribute->getUpdateRange();
			auto data = attribute->getData();

			//绑定当前VBO
			glBindBuffer(toGL(bufferType), dattribute->mHandle);

			//如果用户确实指定的更新的Range
			if (updateRange.mCount > 0) {
				glBufferSubData(
					toGL(bufferType),
					updateRange.mOffset * sizeof(T),
					updateRange.mCount * sizeof(T),
					data.data() + updateRange.mOffset * sizeof(T));
			}
			//如果用户没有指定的更新的Range，则更新整个VBO
			else {
				glBufferData(toGL(bufferType), data.size() * sizeof(T), data.data(), toGL(attribute->getBufferAllocType()));
			}
			glBindBuffer(toGL(bufferType), 0);

			attribute->clearUpdateRange();
		}

		return dattribute;
	}
}