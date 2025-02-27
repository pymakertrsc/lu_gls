#pragma once
#include "base.h"

namespace ff {

	//1 希望注册某个一对象的某一个函数
	//2 希望可以取消掉对某个事件的监听函数

	//收发的消息体
	class EventBase {
	public:
		using Ptr = std::shared_ptr<EventBase>;
		static Ptr create(const std::string& eventName) {
			return std::make_shared <EventBase>(eventName);
		}

		EventBase(const std::string& eventName) noexcept;

		~EventBase() noexcept;

		std::string mEventName;//消息的名字
		void* mTarget = nullptr;//消息的主体，哪个对象发出的,存储其指针
		void* mpUserData{ nullptr };//根据不同的需求，将userData设置成不同的对象指针
	};

	class EventDispatcher {
	public:
		//for class::function
		template<typename T>
		using TypedFunctionPointer = void(T::*)(const EventBase::Ptr&);

		//for wrapper function
		using EventFunction = std::function<void(const EventBase::Ptr&)>;

		struct Listener {
			using Ptr = std::shared_ptr<Listener>;
			static Ptr create() { return std::make_shared<Listener>(); }

			Listener() noexcept {}

			~Listener() noexcept {
				if (mFuncionPointerDescriptor.mAddr) {
					free(mFuncionPointerDescriptor.mAddr);
				}
			}

			struct FunctionPointerDescriptor {

				//函数指针地址的指针！！
				void*	mAddr = nullptr;
				size_t	mSize{ 0 };

				bool operator==(const FunctionPointerDescriptor& other) const {
					if (other.mSize != mSize) {
						return false;
					}

					//按位对比
					if (memcmp(mAddr, other.mAddr, mSize) != 0) {
						return false;
					}

					return true;
				}
			};

			template<typename T>
			static FunctionPointerDescriptor buildFunctionPointer(TypedFunctionPointer<T> functionPointer) noexcept {
				//计算本指针有多大的size,不是指针指向的内容
				size_t s = sizeof(TypedFunctionPointer<T>);
				void* p = malloc(s);

				//placement New 先分配了一块内存，然后通过placementNew调用了其构造函数/默认构造
				new(p)TypedFunctionPointer<T>(functionPointer);

				Listener::FunctionPointerDescriptor fpd;
				fpd.mAddr = p;//p指向的内存当中，存储了functionPointer的指针值
				fpd.mSize = s;//functionPointer的指针值的大小 

				return fpd;
			}

			//caller 
			void* mTarget = nullptr;//某一个对象的this指针

			//functionPointerDescriptor
			FunctionPointerDescriptor mFuncionPointerDescriptor{};

			//functionPointer
			//对函数指针的管理类
			std::function<void(const EventBase::Ptr&)> mFunction = nullptr;
		};

		static bool listenerIsEqual(const Listener::Ptr& l0, const Listener::Ptr& l1) {
			//调用对象的地址一致，且对应的函数地址一致即可 
			return l0->mTarget == l1->mTarget && l0->mFuncionPointerDescriptor == l1->mFuncionPointerDescriptor;
		}

		using ListenerQueue = std::vector<Listener::Ptr>;

		EventDispatcher() noexcept;

		~EventDispatcher() noexcept;

		static EventDispatcher* getInstance();

		template<typename T>
		void addEventListener(const std::string& name, T* target, TypedFunctionPointer<T> functionPointer) noexcept;

		template<typename T>
		void removeEventListener(const std::string& name, T* target, TypedFunctionPointer<T> functionPointer) noexcept;

		void dispatchEvent(const EventBase::Ptr& event);

	protected:
		//存储了监听事件名称——监听函数队列
		//同一个事件名称，可能会有多个Listener监听（多个对象的函数）
		std::unordered_map<std::string, ListenerQueue> mListeners;
		static EventDispatcher* mInstance;
	};

	template<typename T>
	void EventDispatcher::addEventListener(const std::string& name, T* target, TypedFunctionPointer<T> functionPointer) noexcept {
		//queueIter是当前这个消息（name）对应的map当中的，键值对的迭代器，键值对是（string， ListenerQueue）
		//queueIter而言，first是消息名字，second是队列vector<Listener>
		auto queueIter = mListeners.find(name);

		if (queueIter == mListeners.end()) {
			queueIter = (mListeners.insert(std::make_pair(name, ListenerQueue()))).first;
		}

		//Attention！listenerQueue是一个引用！
		auto& listenerQueue = queueIter->second;

		//functionPointer是一个类内的函数指针
		//target是对象的指针
		//bind操作，将二者结合在一起，生成一个function
		auto function = std::bind(functionPointer, target, std::placeholders::_1);

		//开始构建listener
		Listener::Ptr listener = Listener::create();
		listener->mTarget = target;

		//通过bind生成的function无法进行对比，无法使用function1 == function2来判断是否相等
		//传入的functionPointer是bind之前的结果，即单独的函数指针地址
		listener->mFuncionPointerDescriptor = Listener::buildFunctionPointer(functionPointer);
		listener->mFunction = function;

		auto listenerIter = std::find_if(listenerQueue.begin(), listenerQueue.end(),
			[&](const Listener::Ptr& l) {
				return EventDispatcher::listenerIsEqual(listener, l);
			});
		if (listenerIter != listenerQueue.end()) {
			return;
		}

		listenerQueue.push_back(listener);
	}

	template<typename T>
	void EventDispatcher::removeEventListener(const std::string& name, T* target, TypedFunctionPointer<T> functionPointer) noexcept {
		auto queueIter = mListeners.find(name);

		if (queueIter == mListeners.end()) {
			return;
		}

		auto& listenerQueue = queueIter->second;

		auto function = std::bind(functionPointer, target, std::placeholders::_1);
		Listener::Ptr listener = Listener::create();
		listener->mTarget = target;
		listener->mFuncionPointerDescriptor = Listener::buildFunctionPointer(functionPointer);
		listener->mFunction = function;

		auto listenerIter = std::find_if(listenerQueue.begin(), listenerQueue.end(),
			[&](const Listener::Ptr& l) {
				return EventDispatcher::listenerIsEqual(listener, l);
			});
		if (listenerIter != listenerQueue.end()) {
			listenerQueue.erase(listenerIter);
		}
	}
}