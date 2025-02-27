#pragma once
#include "../global/base.h"
#include "../global/constant.h"
#include "../tools/identity.h"
#include "../global/eventDispatcher.h"

namespace ff {
	//对于每个Mesh，我们将其所有顶点的某个Attribute共同存储成一个数组，比如Position就是一个float类型的数组
	//每个Attribute有可能数字类型不同，比如Position需要float，index需要uint32_t
	template<typename T>
	class Attribute :public std::enable_shared_from_this<Attribute<T>> {
		//继承enable_shared_from_this可以允许在类内的函数中，获取到本Attribute对象的智能指针
	public:

		//Ptr是本类型智能指针的别名，为了缩短代码长度
		//create用来创建一个Attribute类型的智能指针的静态函数
		using Ptr = std::shared_ptr<Attribute<T>>;
		static Ptr create(const std::vector<T>& data, uint32_t itemSize, BufferAllocType bufferAllocType = BufferAllocType::StaticDrawBuffer) {
			return std::make_shared <Attribute<T>>(data, itemSize, bufferAllocType);
		}

		Attribute(const std::vector<T>& data, uint32_t itemSize, BufferAllocType bufferAllocType = BufferAllocType::StaticDrawBuffer) noexcept;

		~Attribute() noexcept;

		//将value设置到第index个顶点的x值上
		void setX(const uint32_t& index, T value) noexcept;

		void setY(const uint32_t& index, T value) noexcept;

		void setZ(const uint32_t& index, T value) noexcept;

		//得到第index个顶点的本attribute的x值
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
		ID				mID{ 0 };//全局唯一id
		std::vector<T>	mData{};//数据数组
		uint32_t		mItemSize{ 0 };//多少个数据为一个顶点的Attribute
		uint32_t		mCount{ 0 };//本attribute的数据，包含了多少个顶点的数据

		//opengl或者其他图形api，将buffer分成了两种类型（在咱们使用范围内），
		//static,在创建VBO的时候，会将内存分配到GPU端，适合一次性传输，不经常更改的数据
		//dynamic，在创建VBO的时候，会将内存分配到CPU端（大概率），适合多次频繁的更改
		BufferAllocType	mBufferAllocType{ BufferAllocType::StaticDrawBuffer };

		DataType		mDataType{ DataType::FloatType };//记录本Attribute的数据类型float int uint

		bool			mNeedsUpdate{ true };//数据是否需要更新
		Range			mUpdateRange{};//假设数组长度为300个float类型的数组，本次更新，可以只更新55-100个float数据
	};

	//根据数据类型的不同，起不同的别名
	using Attributef = Attribute<float>;
	using Attributei = Attribute<uint32_t>;

	template<typename T>
	Attribute<T>::Attribute(const std::vector<T>& data, uint32_t itemSize, BufferAllocType bufferAllocType) noexcept {
		mID = Identity::generateID();

		mData = data;
		mItemSize = itemSize;
		//蕴含多少个顶点的信息
		mCount = static_cast<uint32_t>(mData.size()) / itemSize;
		mBufferAllocType = bufferAllocType;

		mDataType = toDataType<T>();
	}

	template<typename T>
	Attribute<T>::~Attribute() noexcept {
		//发送消息，给到各类监听的函数，告诉他们，哪个attribute已经消亡了
		EventBase::Ptr e = EventBase::create("attributeDispose");
		e->mTarget = this;
		e->mpUserData = &mID;

		EventDispatcher::getInstance()->dispatchEvent(e);
	}

	template<typename T>
	void Attribute<T>::setX(const uint32_t& index, T value) noexcept {
		//使用断言来防止index过界
		assert(index < mCount);

		//float vector: a b c value e f g h i j
		//假设index = 1 itemsize=3
		mData[index * mItemSize] = value;
		mNeedsUpdate = true;
	}

	template<typename T>
	void Attribute<T>::setY(const uint32_t& index, T value) noexcept {
		assert(index < mCount);

		//float vector: a b c d value f g h i j
		//假设index = 1 itemsize=3
		mData[index * mItemSize + 1] = x;
		mNeedsUpdate = true;
	}

	template<typename T>
	void Attribute<T>::setZ(const uint32_t& index, T value) noexcept {
		assert(index < mCount);

		//float vector: a b c d e value g h i j
		//假设index = 1 itemsize=3
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
