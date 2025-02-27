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

	//Ѱ�ҵ�ǰgeometry�Ƿ��������ɹ�һ��DriverBindingState�����û�У���������һ��������������Ľ���ȥ
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

		//ʹ��geometryѰ�Ҷ�Ӧ��DriverBindingState������о͸��أ����û�о��������ɣ�����VAOҲ��������һ��
		auto state = getBindingState(geometry);
		if (mCurrentBindingState != state) {
			mCurrentBindingState = state;
			bindVAO(state->mVAO);
		}

		updateBufferLayout = needsUpdate(geometry, index);
		if (updateBufferLayout) {
			saveCache(geometry, index);
		}

		//ע�⣡���ﴦ����indexAttribute��DriverAttribute�Ķ�Ӧ����EBO�Ĵ���
		//indexӦ����vaoƽ������
		if (index != nullptr) {
			mAttributes->update(index, BufferType::IndexBuffer);
		}

		//�����Ҫ���¹ҹ���ϵ����ô��Ҫ��setupVertexAttributes���洦��
		if (updateBufferLayout) {
			setupVertexAttributes(geometry);

			//�����index ����Ҫ����ebo�İ�
			if (index != nullptr) {
				//��DriverAttributes�����ó���indexAttribute��Ӧ��DriverAttribute
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

	//��VAO֮����Ҫʹ��glVertexAttribPointer�Ⱥ�������vao�����vbo֮��Ĺҹ���ϵ������
	//���ǣ������һ֡�������֮�󣬵ڶ�֡��������ͬ����VAO����ô����ҹ���ϵ�����ã��Ͳ���Ҫ���ڶ�����
	//�����������壬�����ڼ�鵱ǰ��vao�Ƿ���Ҫ��������һ�������vbo֮��Ĺҹ���ϵ
	//��Ҫ���¹ҹ��������
	//1 geometry�����attribute���������ı�(������߼��٣�
	//2 geometry�����key����Ӧ��attribute�����˱仯����������setAttribute,����ͬ����key�������µ�attribute
	//��Attribute�������µ�vbo��������Ҫ���°�

	bool DriverBindingStates::needsUpdate(const Geometry::Ptr& geometry, const Attributei::Ptr& index) noexcept {
		//id->���֣�value->attribute id
		auto cachedAttributes = mCurrentBindingState->mAttributes;

		//id->���֣�value->attribute����
		auto geometryAttributes = geometry->getAttributes();

		uint32_t attributesNum = 0;
		for (const auto& iter : geometryAttributes) {
			auto key = iter.first;
			auto geometryAttribute = iter.second;

			//�ӻ�������Ѱ�ң�������һ��attributeû�ҵ���˵���Ͳ�һ����
			auto cachedIter = cachedAttributes.find(key);
			if (cachedIter == cachedAttributes.end()) {
				return true;
			}

			//�ӻ��浱�У�ȷʵ�ҵ������attribute����ô�͵öԱ�id�Ƿ�һ����
			//�Ӷ�ȷ����ͬ�����ֵ�attribute�Ƿ���ͬһ��
			auto cachedAttribute = cachedIter->second;
			if (cachedAttribute != geometryAttribute->getID()) {
				return true;
			}

			attributesNum++;
		}

		//����������ɵ�geometry��3�����ԣ��µ�geometryȥ����һ������ʣ��2����
		if (mCurrentBindingState->mAttributesNum != attributesNum) {
			return true;
		}

		//indexAttribute �����ͬ����Ȼ��Ҫ���¹ҹ�
		if (index != nullptr && mCurrentBindingState->mIndex != index->getID()) {
			return true;
		}

		//������������һ�£���ô��˵����geometry��û�б仯���򷵻ز���Ҫ���¹ҹ�
		return false;
	}

	void DriverBindingStates::saveCache(const Geometry::Ptr& geometry, const Attributei::Ptr& index) noexcept {
		//������յ�bindingState�����attributes ��Map��
		auto& cachedAttributes = mCurrentBindingState->mAttributes;
		cachedAttributes.clear();

		auto attributes = geometry->getAttributes();
		uint32_t attributesNum = 0;

		//����geometry��ÿһ��attribute
		for (const auto& iter : attributes) {
			auto attribute = iter.second;

			//����key-attributeName��value-attributeID
			cachedAttributes.insert(std::make_pair(iter.first, attribute->getID()));
			attributesNum++;
		}

		mCurrentBindingState->mAttributesNum = attributesNum;

		if (index != nullptr) {
			mCurrentBindingState->mIndex = index->getID();
		}
	}

	//�ҷ���������˼·��
	//��positionAttribute��Զlocation = 0
	//��normalAttribute��Զlocation = 1....
	//��ǰ��ƺõ�ռ�ӷ���
	void DriverBindingStates::setupVertexAttributes(const Geometry::Ptr& geometry) noexcept {
		const auto geometryAttributes = geometry->getAttributes();

		for (const auto& iter : geometryAttributes) {
			auto name = iter.first;
			auto attribute = iter.second;

			//itemSize��attribute�ж��ٸ����֣�����position����3������
			auto itemSize = attribute->getItemSize();

			//ÿ�����������ݵ�����
			auto dataType = attribute->getDataType();

			//��attribute��Ӧ��DriverAttributeȡ����
			auto bkAttribute = mAttributes->get(attribute);

			//����attribute��location(binding)ͨ��attribute��nameȡ����
			auto bindingIter = LOCATION_MAP.find(name);
			if (bindingIter == LOCATION_MAP.end()) {
				continue;
			}

			auto binding = bindingIter->second;

			//��ʼ��vao�������ҹ���ϵ
			glBindBuffer(GL_ARRAY_BUFFER, bkAttribute->mHandle);
			//�����Ӧ��binding��
			glEnableVertexAttribArray(binding);
			//��vao�����¼�����ڱ�binding������Ӧ��attribute������Ӧ����δ�bkAttribute->mHandleһ��vbo�����ȡ����
			glVertexAttribPointer(binding, itemSize, toGL(dataType), false, itemSize * toSize(dataType), (void*)0);
		}
	}

	//������������һ��VAO
	GLuint DriverBindingStates::createVAO() noexcept {
		GLuint vao = 0;
		glGenVertexArrays(1, &vao);
		return vao;
	}

	// �����ı���OpenGL״̬����ʹ֮�󶨵�ǰ��vao
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