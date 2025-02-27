#pragma once
#include "../../global/base.h"
#include "../../core/attribute.h"
#include "../../global/eventDispatcher.h"

namespace ff {

	//DriverAttribute ����Attributeһһ��Ӧ
	class DriverAttribute {
	public:
		using Ptr = std::shared_ptr<DriverAttribute>;
		static Ptr create() {
			return std::make_shared <DriverAttribute>();
		}

		DriverAttribute() noexcept;

		~DriverAttribute() noexcept;

		//mHandle����VBO
		//VBO����OpenGL��һ�����VertexBufferObject����������GPU �ϵ�һ���ڴ�ռ䣬ר������
		//�洢Mesh�����Attribute����
		GLuint		mHandle{ 0 };
	};

	class DriverAttributes {
	public:
		using Ptr = std::shared_ptr<DriverAttributes>;
		static Ptr create() {
			return std::make_shared <DriverAttributes>();
		}

		//ID:ǰ�˵�Attribute��ID
		//Value����ID����Ӧ��Attribute���ɵ�DriverAttribute
		using DriverAttributesMap = std::unordered_map<ID, DriverAttribute::Ptr>;
		DriverAttributes() noexcept;

		~DriverAttributes() noexcept;

		//���ܣ�
		// 1 �����Attributeû�ж�Ӧ��DriverAttribute����Ϊ�����ɣ��Ҹ�������
		// 2 �����Attributeӵ��һ����Ӧ��DriverAttribute��������VBO �����Ƿ���Ҫ����
		//bufferType��ʾ�˱�Attribute��һ��indexAttribute������һ����ͨ��Attribute
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

		//Ѱ�ң������Ƿ����ǵ�map���б�Attribute��Ӧ��DriverAttribute
		auto iter = mAttributes.find(attribute->getID());
		if (iter != mAttributes.end()) {
			dattribute = iter->second;
		}
		else {

			//���û���ҵ����򴴽�һ���µ�DriverAttribute
			dattribute = DriverAttribute::create();

			//�õ�Attribute���������
			auto data = attribute->getData();

			//Ϊ��Attribute��Ӧ��DriverAttribute����VBO ���Ҹ�������
			glGenBuffers(1, &dattribute->mHandle);

			//bufferTypeҪô��GL_ARRAY_BUFFER Ҫô�� GL_ELEMENT_ARRAY_BUFFER
			glBindBuffer(toGL(bufferType), dattribute->mHandle);

			//VBO�ڴ濪�٣��Լ�VBO ���ݵĹ���
			glBufferData(toGL(bufferType), data.size() * sizeof(T), data.data(), toGL(attribute->getBufferAllocType()));
			glBindBuffer(toGL(bufferType), 0);

			mAttributes.insert(std::make_pair(attribute->getID(), dattribute));

			//��Attribute����Ҫ���µ�״̬���
			attribute->clearUpdateRange();
			attribute->clearNeedsUpdate();
		}

		//���ԭ���ʹ���DriverAttribute����ô�͵ü���Ƿ���Ҫ����
		if (attribute->getNeedsUpdate()) {
			attribute->clearNeedsUpdate();

			//��ȡ���µ�offset�Լ�Count
			auto updateRange = attribute->getUpdateRange();
			auto data = attribute->getData();

			//�󶨵�ǰVBO
			glBindBuffer(toGL(bufferType), dattribute->mHandle);

			//����û�ȷʵָ���ĸ��µ�Range
			if (updateRange.mCount > 0) {
				glBufferSubData(
					toGL(bufferType),
					updateRange.mOffset * sizeof(T),
					updateRange.mCount * sizeof(T),
					data.data() + updateRange.mOffset * sizeof(T));
			}
			//����û�û��ָ���ĸ��µ�Range�����������VBO
			else {
				glBufferData(toGL(bufferType), data.size() * sizeof(T), data.data(), toGL(attribute->getBufferAllocType()));
			}
			glBindBuffer(toGL(bufferType), 0);

			attribute->clearUpdateRange();
		}

		return dattribute;
	}
}