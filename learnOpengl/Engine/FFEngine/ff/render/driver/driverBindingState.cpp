#include "driverBindingState.h"
#include "../../core/geometry.h"

namespace ff {

	DriverBindingState::DriverBindingState() noexcept {}

	DriverBindingState::~DriverBindingState() noexcept {
		if (mVAO) {
			glDeleteVertexArrays(1, &mVAO);
		}
	}

	DriverBindingStates::DriverBindingStates(const DriverAttributes::Ptr& attributes) {
		mAttributes = attributes;
	}

	DriverBindingStates::~DriverBindingStates() {}

	//寻找当前geometry是否曾经生成过一个DriverBindingState，如果没有，则新生成一个，否则把以往的交回去
	DriverBindingState::Ptr DriverBindingStates::getBindingState(const Geometry::Ptr& geometry) noexcept {
		DriverBindingState::Ptr state = nullptr;

		auto gKeyIter = mBindingStates.find(geometry->getID());
		if (gKeyIter == mBindingStates.end()) {
			gKeyIter = mBindingStates.insert(std::make_pair(geometry->getID(), createBindingState(createVAO()))).first;
		}
	
		state = gKeyIter->second;

		return state;
	}

	void DriverBindingStates::setup(
		const Geometry::Ptr& geometry,
		const Attributei::Ptr& index
	) {
		bool updateBufferLayout = false;

		//使用geometry寻找对应的DriverBindingState，如果有就给回；如果没有就重新生成，并且VAO也连带生成一个
		auto state = getBindingState(geometry);
		if (mCurrentBindingState != state) {
			mCurrentBindingState = state;
			bindVAO(state->mVAO);
		}

		updateBufferLayout = needsUpdate(geometry, index);
		if (updateBufferLayout) {
			saveCache(geometry, index);
		}

		//注意！这里处理了indexAttribute到DriverAttribute的对应，即EBO的创建
		//index应该与vao平级处理
		if (index != nullptr) {
			mAttributes->update(index, BufferType::IndexBuffer);
		}

		//如果需要更新挂钩关系，那么就要在setupVertexAttributes里面处理
		if (updateBufferLayout) {
			setupVertexAttributes(geometry);

			//如果有index 则需要进行ebo的绑定
			if (index != nullptr) {
				//从DriverAttributes里面拿出来indexAttribute对应的DriverAttribute
				auto bkIndex = mAttributes->get(index);
				if (bkIndex != nullptr) {
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bkIndex->mHandle);
				}

			}
		}
	}

	DriverBindingState::Ptr DriverBindingStates::createBindingState(GLuint vao) noexcept {
		auto bindingState = DriverBindingState::create();
		bindingState->mVAO = vao;

		return bindingState;
	}

	//绑定VAO之后，需要使用glVertexAttribPointer等函数，对vao与各个vbo之间的挂钩关系做设置
	//但是，如果第一帧绘制完毕之后，第二帧继续绘制同样的VAO，那么这个挂钩关系的设置，就不需要做第二次啦
	//本函数的意义，就在于检查当前的vao是否需要重新设置一次与各个vbo之间的挂钩关系
	//需要重新挂钩的情况：
	//1 geometry里面的attribute数量发生改变(增多或者减少）
	//2 geometry里面的key所对应的attribute发生了变化，即调用了setAttribute,由于同样的key更换了新的attribute
	//则本Attribute会生成新的vbo，所以需要重新绑定

	bool DriverBindingStates::needsUpdate(const Geometry::Ptr& geometry, const Attributei::Ptr& index) noexcept {
		//id->名字，value->attribute id
		auto cachedAttributes = mCurrentBindingState->mAttributes;

		//id->名字，value->attribute对象
		auto geometryAttributes = geometry->getAttributes();

		uint32_t attributesNum = 0;
		for (const auto& iter : geometryAttributes) {
			auto key = iter.first;
			auto geometryAttribute = iter.second;

			//从缓存里面寻找，但凡有一个attribute没找到，说明就不一样了
			auto cachedIter = cachedAttributes.find(key);
			if (cachedIter == cachedAttributes.end()) {
				return true;
			}

			//从缓存当中，确实找到了这个attribute，那么就得对比id是否一致了
			//从而确定，同样名字的attribute是否是同一个
			auto cachedAttribute = cachedIter->second;
			if (cachedAttribute != geometryAttribute->getID()) {
				return true;
			}

			attributesNum++;
		}

		//举例：如果旧的geometry有3个属性，新的geometry去掉了一个，就剩下2个了
		if (mCurrentBindingState->mAttributesNum != attributesNum) {
			return true;
		}

		//indexAttribute 如果不同，仍然需要重新挂钩
		if (index != nullptr && mCurrentBindingState->mIndex != index->getID()) {
			return true;
		}

		//如果上述结果都一致，那么就说明本geometry并没有变化，则返回不需要重新挂勾
		return false;
	}

	void DriverBindingStates::saveCache(const Geometry::Ptr& geometry, const Attributei::Ptr& index) noexcept {
		//首先清空掉bindingState里面的attributes （Map）
		auto& cachedAttributes = mCurrentBindingState->mAttributes;
		cachedAttributes.clear();

		auto attributes = geometry->getAttributes();
		uint32_t attributesNum = 0;

		//遍历geometry的每一个attribute
		for (const auto& iter : attributes) {
			auto attribute = iter.second;

			//插入key-attributeName，value-attributeID
			cachedAttributes.insert(std::make_pair(iter.first, attribute->getID()));
			attributesNum++;
		}

		mCurrentBindingState->mAttributesNum = attributesNum;

		if (index != nullptr) {
			mCurrentBindingState->mIndex = index->getID();
		}
	}

	//我方引擎的设计思路：
	//令positionAttribute永远location = 0
	//令normalAttribute永远location = 1....
	//提前设计好的占坑方案
	void DriverBindingStates::setupVertexAttributes(const Geometry::Ptr& geometry) noexcept {
		const auto geometryAttributes = geometry->getAttributes();

		for (const auto& iter : geometryAttributes) {
			auto name = iter.first;
			auto attribute = iter.second;

			//itemSize本attribute有多少个数字，比如position就是3个数字
			auto itemSize = attribute->getItemSize();

			//每个单独的数据的类型
			auto dataType = attribute->getDataType();

			//将attribute对应的DriverAttribute取出来
			auto bkAttribute = mAttributes->get(attribute);

			//将本attribute的location(binding)通过attribute的name取出来
			auto bindingIter = LOCATION_MAP.find(name);
			if (bindingIter == LOCATION_MAP.end()) {
				continue;
			}

			auto binding = bindingIter->second;

			//开始向vao里面做挂钩关系
			glBindBuffer(GL_ARRAY_BUFFER, bkAttribute->mHandle);
			//激活对应的binding点
			glEnableVertexAttribArray(binding);
			//向vao里面记录，对于本binding点所对应的attribute，我们应该如何从bkAttribute->mHandle一个vbo里面读取数据
			glVertexAttribPointer(binding, itemSize, toGL(dataType), false, itemSize * toSize(dataType), (void*)0);
		}
	}

	//真正的生成了一个VAO
	GLuint DriverBindingStates::createVAO() noexcept {
		GLuint vao = 0;
		glGenVertexArrays(1, &vao);
		return vao;
	}

	// 真正改变了OpenGL状态机，使之绑定当前的vao
	void DriverBindingStates::bindVAO(GLuint vao) noexcept {
		glBindVertexArray(vao);
	}

	void DriverBindingStates::releaseStatesOfGeometry(ID geometryID) noexcept {
		auto iter = mBindingStates.find(geometryID);
		if (iter != mBindingStates.end()) {
			mBindingStates.erase(iter);
		}
	}

}