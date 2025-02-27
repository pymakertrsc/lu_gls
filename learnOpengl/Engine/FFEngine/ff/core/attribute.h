#pragma once
#include "../global/base.h"
#include "../global/constant.h"
#include "../tools/identity.h"
#include "../global/eventDispatcher.h"

namespace ff {
	//����ÿ��Mesh�����ǽ������ж����ĳ��Attribute��ͬ�洢��һ�����飬����Position����һ��float���͵�����
	//ÿ��Attribute�п����������Ͳ�ͬ������Position��Ҫfloat��index��Ҫuint32_t
	template<typename T>
	class Attribute :public std::enable_shared_from_this<Attribute<T>> {
		//�̳�enable_shared_from_this�������������ڵĺ����У���ȡ����Attribute���������ָ��
	public:

		//Ptr�Ǳ���������ָ��ı�����Ϊ�����̴��볤��
		//create��������һ��Attribute���͵�����ָ��ľ�̬����
		using Ptr = std::shared_ptr<Attribute<T>>;
		static Ptr create(const std::vector<T>& data, uint32_t itemSize, BufferAllocType bufferAllocType = BufferAllocType::StaticDrawBuffer) {
			return std::make_shared <Attribute<T>>(data, itemSize, bufferAllocType);
		}

		Attribute(const std::vector<T>& data, uint32_t itemSize, BufferAllocType bufferAllocType = BufferAllocType::StaticDrawBuffer) noexcept;

		~Attribute() noexcept;

		//��value���õ���index�������xֵ��
		void setX(const uint32_t& index, T value) noexcept;

		void setY(const uint32_t& index, T value) noexcept;

		void setZ(const uint32_t& index, T value) noexcept;

		//�õ���index������ı�attribute��xֵ
		T getX(const uint32_t& index) noexcept;

		T getY(const uint32_t& index) noexcept;

		T getZ(const uint32_t& index) noexcept;

		auto getID() const noexcept { return mID; }

		auto getData() const noexcept { return mData; }

		auto getCount() const noexcept { return mCount; }

		auto getItemSize() const noexcept { return mItemSize; }

		bool getNeedsUpdate() const noexcept { return mNeedsUpdate; }

		void clearNeedsUpdate() noexcept { mNeedsUpdate = false; }

		auto getBufferAllocType() const noexcept { return mBufferAllocType; }

		Range getUpdateRange() const noexcept { return mUpdateRange; }

		void clearUpdateRange() noexcept { mUpdateRange.mOffset = 0; mUpdateRange.mCount = -1; }

		auto getDataType() const noexcept { return mDataType; }

	private:
		ID				mID{ 0 };//ȫ��Ψһid
		std::vector<T>	mData{};//��������
		uint32_t		mItemSize{ 0 };//���ٸ�����Ϊһ�������Attribute
		uint32_t		mCount{ 0 };//��attribute�����ݣ������˶��ٸ����������

		//opengl��������ͼ��api����buffer�ֳ����������ͣ�������ʹ�÷�Χ�ڣ���
		//static,�ڴ���VBO��ʱ�򣬻Ὣ�ڴ���䵽GPU�ˣ��ʺ�һ���Դ��䣬���������ĵ�����
		//dynamic���ڴ���VBO��ʱ�򣬻Ὣ�ڴ���䵽CPU�ˣ�����ʣ����ʺ϶��Ƶ���ĸ���
		BufferAllocType	mBufferAllocType{ BufferAllocType::StaticDrawBuffer };

		DataType		mDataType{ DataType::FloatType };//��¼��Attribute����������float int uint

		bool			mNeedsUpdate{ true };//�����Ƿ���Ҫ����
		Range			mUpdateRange{};//�������鳤��Ϊ300��float���͵����飬���θ��£�����ֻ����55-100��float����
	};

	//�����������͵Ĳ�ͬ����ͬ�ı���
	using Attributef = Attribute<float>;
	using Attributei = Attribute<uint32_t>;

	template<typename T>
	Attribute<T>::Attribute(const std::vector<T>& data, uint32_t itemSize, BufferAllocType bufferAllocType) noexcept {
		mID = Identity::generateID();

		mData = data;
		mItemSize = itemSize;
		//�̺����ٸ��������Ϣ
		mCount = static_cast<uint32_t>(mData.size()) / itemSize;
		mBufferAllocType = bufferAllocType;

		mDataType = toDataType<T>();
	}

	template<typename T>
	Attribute<T>::~Attribute() noexcept {
		//������Ϣ��������������ĺ������������ǣ��ĸ�attribute�Ѿ�������
		EventBase::Ptr e = EventBase::create("attributeDispose");
		e->mTarget = this;
		e->mpUserData = &mID;

		EventDispatcher::getInstance()->dispatchEvent(e);
	}

	template<typename T>
	void Attribute<T>::setX(const uint32_t& index, T value) noexcept {
		//ʹ�ö�������ֹindex����
		assert(index < mCount);

		//float vector: a b c value e f g h i j
		//����index = 1 itemsize=3
		mData[index * mItemSize] = value;
		mNeedsUpdate = true;
	}

	template<typename T>
	void Attribute<T>::setY(const uint32_t& index, T value) noexcept {
		assert(index < mCount);

		//float vector: a b c d value f g h i j
		//����index = 1 itemsize=3
		mData[index * mItemSize + 1] = x;
		mNeedsUpdate = true;
	}

	template<typename T>
	void Attribute<T>::setZ(const uint32_t& index, T value) noexcept {
		assert(index < mCount);

		//float vector: a b c d e value g h i j
		//����index = 1 itemsize=3
		mData[index * mItemSize + 2] = x;
		mNeedsUpdate = true;
	}

	template<typename T>
	T Attribute<T>::getX(const uint32_t& index) noexcept {
		assert(index < mCount);
		return mData[index * mItemSize];
	}

	template<typename T>
	T Attribute<T>::getY(const uint32_t& index) noexcept {
		assert(index < mCount);
		return mData[index * mItemSize + 1];
	}

	template<typename T>
	T Attribute<T>::getZ(const uint32_t& index) noexcept {
		assert(index < mCount);
		return mData[index * mItemSize + 2];
	}
}
