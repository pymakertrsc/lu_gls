#include "object3D.h"
#include "../tools/identity.h"
#include "../global/eventDispatcher.h"

namespace ff {

	Object3D::Object3D() noexcept {
		mID = Identity::generateID();
	}

	Object3D::~Object3D() noexcept {
		EventBase::Ptr e = EventBase::create("objectDispose");
		e->mTarget = this;
		EventDispatcher::getInstance()->dispatchEvent(e);
	}

	void Object3D::setPosition(float x, float y, float z) noexcept {
		setPosition(glm::vec3(x, y, z));	
	}

	void Object3D::setPosition(const glm::vec3& position) noexcept {
		//��glm����£������ȴ洢:a e i m b f j n c g k o d h l p
		//��glm����£������ȴ洢:a b c d e f g h i j....
		//a b c d
		//e f g h
		//i j k l
		//m n o p
		//��ȡ������
		mLocalMatrix[3].x = position.x;
		mLocalMatrix[3].y = position.y;
		mLocalMatrix[3].z = position.z;

		mPosition = position;
	}

	void Object3D::setQuaternion(float x, float y, float z, float w) noexcept {
		//����Ԫ������£�glm�ĳ�ʼ����w xyz
		glm::quat quaternion(w, x, y, z);
	
		//���ǵ���localMatrix�����Ѿ���ʩ����scale����ı任
		float scaleX = glm::length(glm::vec3(mLocalMatrix[0]));
		float scaleY = glm::length(glm::vec3(mLocalMatrix[1]));
		float scaleZ = glm::length(glm::vec3(mLocalMatrix[2]));
 
		//��glm����Ԫ��ת��Ϊһ����ת����
		glm::mat4 rotateMatrix = glm::mat4_cast(quaternion);

		//��scale�任�ָ���ȥ
		mLocalMatrix[0] = rotateMatrix[0] * scaleX;
		mLocalMatrix[1] = rotateMatrix[1] * scaleY;
		mLocalMatrix[2] = rotateMatrix[2] * scaleX;
	
		decompose(); 
	}

	void Object3D::setScale(float x, float y, float z) noexcept {

		//�õ�ĳһ�У�normalizeȥ��֮ǰ��scaleӰ��,�ٳ��Ե�ǰ�����scale
		auto col0 = glm::normalize(glm::vec3(mLocalMatrix[0])) * x;
		auto col1 = glm::normalize(glm::vec3(mLocalMatrix[1])) * y;
		auto col2 = glm::normalize(glm::vec3(mLocalMatrix[2])) * z;

		//�����úõ�ǰ���У��������õ�localmatrix
		mLocalMatrix[0] = glm::vec4(col0, 0.0f);
		mLocalMatrix[1] = glm::vec4(col1, 0.0f);
		mLocalMatrix[2] = glm::vec4(col2, 0.0f);
		
		decompose();
	}


	void Object3D::rotateX(float angle) noexcept {
		//���Ȼ�ȡ����ǰģ��״̬�µ��Ҳ෽��
		glm::vec3 rotateAxis = glm::vec3(mLocalMatrix[0]);

		//�������Ҳ෽����Ϊ��ת����������ת,angle���������壬��angle>0, ���������ת�ῴ����ʱ����ת
		glm::mat4 rotateMatrix = glm::rotate(glm::mat4(1.0), glm::radians(angle), rotateAxis);
		mLocalMatrix = rotateMatrix * mLocalMatrix;

		decompose();
	}

	void Object3D::rotateY(float angle) noexcept {
		glm::vec3 rotateAxis = glm::vec3(mLocalMatrix[1]);
		glm::mat4 rotateMatrix = glm::rotate(glm::mat4(1.0), glm::radians(angle), rotateAxis);
		mLocalMatrix = rotateMatrix * mLocalMatrix;

		decompose();
	}

	void Object3D::rotateZ(float angle) noexcept {
		glm::vec3 rotateAxis = glm::vec3(mLocalMatrix[2]);
		glm::mat4 rotateMatrix = glm::rotate(glm::mat4(1.0), glm::radians(angle), rotateAxis);
		mLocalMatrix = rotateMatrix * mLocalMatrix;

		decompose();
	}

	//����һ�εĻ�������ת
	void Object3D::rotateAroundAxis(const glm::vec3& axis, float angle) noexcept {
		mLocalMatrix = glm::rotate(mLocalMatrix, glm::radians(angle), axis);

		decompose();
	}

	//����ת��գ�������ת
	void Object3D::setRotateAroundAxis(const glm::vec3& axis, float angle) noexcept {
		glm::mat4 rotateMatrix = glm::rotate(glm::mat4(1.0), glm::radians(angle), axis);

		//decompose scale
		float scaleX = glm::length(glm::vec3(mLocalMatrix[0]));
		float scaleY = glm::length(glm::vec3(mLocalMatrix[1]));
		float scaleZ = glm::length(glm::vec3(mLocalMatrix[2]));
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0), glm::vec3(scaleX, scaleY, scaleZ));

		//�����
		mLocalMatrix[0] = rotateMatrix[0];
		mLocalMatrix[1] = rotateMatrix[1];
		mLocalMatrix[2] = rotateMatrix[2];

		mLocalMatrix *= scaleMatrix;

		decompose();
	}

	void Object3D::lookAt(const glm::vec3& target, const glm::vec3& up) noexcept {
		//decompose scale
		float scaleX = glm::length(glm::vec3(mLocalMatrix[0]));
		float scaleY = glm::length(glm::vec3(mLocalMatrix[1]));
		float scaleZ = glm::length(glm::vec3(mLocalMatrix[2]));

		glm::vec3 position = glm::vec3(mLocalMatrix[3]);

		//make local coordinate
		auto nTarget = glm::normalize(target - position) * scaleZ;
		auto nRight = glm::normalize(glm::cross(up, -nTarget)) * scaleX;
		auto nUp = glm::normalize(glm::cross(nRight, nTarget)) * scaleY;


		mLocalMatrix[0] = glm::vec4(nRight, 0.0f);
		mLocalMatrix[1] = glm::vec4(nUp, 0.0f);
		mLocalMatrix[2] = glm::vec4(-nTarget, 0.0f);
		mLocalMatrix[3] = glm::vec4(position, 1.0f);

		decompose();
	}

	void Object3D::setLocalMatrix(const glm::mat4& localMatrix) noexcept {
		mLocalMatrix = localMatrix;

		decompose();
	}

	void Object3D::setWorldMatrix(const glm::mat4& worldMatrix) noexcept {
		mWorldMatrix = worldMatrix;
	}

	void Object3D::updateMatrix() noexcept {
		if (mNeedsUpdateMatrix) {
			mNeedsUpdateMatrix = false;
			auto translateMatrix = glm::translate(glm::mat4(1.0f), mPosition);
			auto rotateMatrix = glm::mat4_cast(mQuaternion);
			auto scaleMatrix = glm::scale(glm::mat4(1.0f), mScale);

			mLocalMatrix = translateMatrix * rotateMatrix * scaleMatrix;
		}
	}

	//ͨ���㼶matrix��ˣ��õ�����ת������������ϵ�ľ���
	glm::mat4 Object3D::updateWorldMatrix(bool updateParent, bool updateChildren) noexcept {

		//�����û�и��ڵ�
		if (!mParent.expired() && updateParent) {
			auto parent = mParent.lock();//�õ����ڵ��sharedPtr
			parent->updateWorldMatrix(true, false);//���ø��ڵ��worldMatrix��������
		}

		//�����Լ���localMatrix 
		updateMatrix();

		//��ʼ��worldMatrix�����û�и��ڵ㣬��ô�������
		mWorldMatrix = mLocalMatrix;

		//����и��ڵ㣬��ô��Ҫ���ɸ��ڵ��worldMatrix���Ӷ����Ϸ����нڵ��Ӱ�����
		if (!mParent.expired()) {
			auto parent = mParent.lock();
			mWorldMatrix = parent->mWorldMatrix * mWorldMatrix;
		}

		//���θ����ӽڵ��worldMatrix
		if (updateChildren) {
			for (auto& child : mChildren) {
				child->updateWorldMatrix(false, true);
			}
		}

		return mWorldMatrix;
	}

	//���������viewMatrix
	glm::mat4 Object3D::updateModelViewMatrix(const glm::mat4& viewMatrix) noexcept {
		mModelViewMatrix = viewMatrix * mWorldMatrix;

		return mModelViewMatrix;
	}

	glm::mat3 Object3D::updateNormalMatrix() noexcept {
		//normalMatrix ���ڴ���scale��Ӱ�죬����ֱ�ӱ任������normal���޷���֤��ֱ�ڱ���
		mNormalMatrix = glm::transpose(glm::inverse(glm::mat3(mModelViewMatrix)));

		return mNormalMatrix;
	}

	glm::vec3 Object3D::getPosition() const noexcept {
		return glm::vec3(mLocalMatrix[3]);
	}

	glm::vec3 Object3D::getWorldPosition() const noexcept {
		return glm::vec3(mWorldMatrix[3]);
	}

	glm::vec3 Object3D::getLocalDirection() const noexcept {
		return glm::normalize(-glm::vec3(mLocalMatrix[2]));
	}

	glm::vec3 Object3D::getWorldDirection() const noexcept {
		return glm::normalize(-glm::vec3(mWorldMatrix[2]));
	}

	glm::vec3 Object3D::getUp() const noexcept {
		return glm::normalize(glm::vec3(mLocalMatrix[1]));
	}

	glm::vec3 Object3D::getRight() const noexcept {
		return glm::normalize(glm::vec3(mLocalMatrix[0]));
	}

	glm::mat4 Object3D::getLocalMatrix() noexcept {
		return mLocalMatrix;
	}

	glm::mat4 Object3D::getWorldMatrix() noexcept {
		return mWorldMatrix;
	}

	glm::mat4 Object3D::getModelViewMatrix() noexcept {
		return mModelViewMatrix;
	}

	glm::mat3 Object3D::getNormalMatrix() noexcept {
		return mNormalMatrix;
	}

	void Object3D::addChild(const Object3D::Ptr& child) noexcept {
		//����ȷ�ϼ�����ӽڵ㲢�����Լ�
		if (child == shared_from_this()) {
			return;
		}

		//����child����һ�����������ָ�룬child����WeakPtr�����ɵģ�������ѭ������
		child->mParent = shared_from_this();

		//���ҵ�ǰ�ڵ��£��Ƿ��Ѿ������˱�Child
		auto iter = std::find(mChildren.begin(), mChildren.end(), child);

		//iterator������������ҵ����ͻ᷵�ص�ǰ���child��Ӧ����������ֵͬ��iterator
		if (iter != mChildren.end()) return;

		mChildren.push_back(child);
	}

	const std::vector<Object3D::Ptr>& Object3D::getChildren() const noexcept {
		return mChildren;
	}

	ID Object3D::getID() const noexcept {
		return mID;
	}

	void Object3D::decompose() noexcept {
		glm::vec3 skew;
		glm::vec4 perspective;

		//�ǽ��任�����еĲ����ǣ�������� 
		glm::decompose(mLocalMatrix, mScale, mQuaternion, mPosition,skew, perspective);
	}
}