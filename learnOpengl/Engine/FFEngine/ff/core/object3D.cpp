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
		//在glm情况下，列优先存储:a e i m b f j n c g k o d h l p
		//在glm情况下，列优先存储:a b c d e f g h i j....
		//a b c d
		//e f g h
		//i j k l
		//m n o p
		//获取第三列
		mLocalMatrix[3].x = position.x;
		mLocalMatrix[3].y = position.y;
		mLocalMatrix[3].z = position.z;

		mPosition = position;
	}

	void Object3D::setQuaternion(float x, float y, float z, float w) noexcept {
		//在四元数情况下，glm的初始化，w xyz
		glm::quat quaternion(w, x, y, z);
	
		//考虑到，localMatrix可能已经被施加了scale方面的变换
		float scaleX = glm::length(glm::vec3(mLocalMatrix[0]));
		float scaleY = glm::length(glm::vec3(mLocalMatrix[1]));
		float scaleZ = glm::length(glm::vec3(mLocalMatrix[2]));
 
		//将glm的四元数转换为一个旋转矩阵
		glm::mat4 rotateMatrix = glm::mat4_cast(quaternion);

		//将scale变换恢复进去
		mLocalMatrix[0] = rotateMatrix[0] * scaleX;
		mLocalMatrix[1] = rotateMatrix[1] * scaleY;
		mLocalMatrix[2] = rotateMatrix[2] * scaleX;
	
		decompose(); 
	}

	void Object3D::setScale(float x, float y, float z) noexcept {

		//拿到某一列，normalize去掉之前的scale影响,再乘以当前的相关scale
		auto col0 = glm::normalize(glm::vec3(mLocalMatrix[0])) * x;
		auto col1 = glm::normalize(glm::vec3(mLocalMatrix[1])) * y;
		auto col2 = glm::normalize(glm::vec3(mLocalMatrix[2])) * z;

		//将设置好的前三列，重新设置到localmatrix
		mLocalMatrix[0] = glm::vec4(col0, 0.0f);
		mLocalMatrix[1] = glm::vec4(col1, 0.0f);
		mLocalMatrix[2] = glm::vec4(col2, 0.0f);
		
		decompose();
	}


	void Object3D::rotateX(float angle) noexcept {
		//首先获取到当前模型状态下的右侧方向
		glm::vec3 rotateAxis = glm::vec3(mLocalMatrix[0]);

		//针对这个右侧方向作为旋转轴来进行旋转,angle正负有意义，当angle>0, 当你冲着旋转轴看，逆时针旋转
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

	//再上一次的基础上旋转
	void Object3D::rotateAroundAxis(const glm::vec3& axis, float angle) noexcept {
		mLocalMatrix = glm::rotate(mLocalMatrix, glm::radians(angle), axis);

		decompose();
	}

	//将旋转清空，重新旋转
	void Object3D::setRotateAroundAxis(const glm::vec3& axis, float angle) noexcept {
		glm::mat4 rotateMatrix = glm::rotate(glm::mat4(1.0), glm::radians(angle), axis);

		//decompose scale
		float scaleX = glm::length(glm::vec3(mLocalMatrix[0]));
		float scaleY = glm::length(glm::vec3(mLocalMatrix[1]));
		float scaleZ = glm::length(glm::vec3(mLocalMatrix[2]));
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0), glm::vec3(scaleX, scaleY, scaleZ));

		//列相等
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

	//通过层级matrix相乘，得到最后的转换到世界坐标系的矩阵
	glm::mat4 Object3D::updateWorldMatrix(bool updateParent, bool updateChildren) noexcept {

		//检查有没有父节点
		if (!mParent.expired() && updateParent) {
			auto parent = mParent.lock();//拿到父节点的sharedPtr
			parent->updateWorldMatrix(true, false);//调用父节点的worldMatrix升级更新
		}

		//更新自己的localMatrix 
		updateMatrix();

		//初始化worldMatrix，如果没有父节点，那么二者相等
		mWorldMatrix = mLocalMatrix;

		//如果有父节点，那么需要做成父节点的worldMatrix，从而把上方所有节点的影响带入
		if (!mParent.expired()) {
			auto parent = mParent.lock();
			mWorldMatrix = parent->mWorldMatrix * mWorldMatrix;
		}

		//依次更新子节点的worldMatrix
		if (updateChildren) {
			for (auto& child : mChildren) {
				child->updateWorldMatrix(false, true);
			}
		}

		return mWorldMatrix;
	}

	//传入摄像机viewMatrix
	glm::mat4 Object3D::updateModelViewMatrix(const glm::mat4& viewMatrix) noexcept {
		mModelViewMatrix = viewMatrix * mWorldMatrix;

		return mModelViewMatrix;
	}

	glm::mat3 Object3D::updateNormalMatrix() noexcept {
		//normalMatrix 由于存在scale的影响，不能直接变换，否则normal会无法保证垂直于表面
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
		//首先确认加入的子节点并不是自己
		if (child == shared_from_this()) {
			return;
		}

		//给到child的是一个自身的智能指针，child是用WeakPtr来接纳的，避免了循环引用
		child->mParent = shared_from_this();

		//查找当前节点下，是否已经加入了本Child
		auto iter = std::find(mChildren.begin(), mChildren.end(), child);

		//iterator迭代器，如果找到，就会返回当前这个child对应到数组中相同值的iterator
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

		//是将变换矩阵当中的参数们，抽离出来 
		glm::decompose(mLocalMatrix, mScale, mQuaternion, mPosition,skew, perspective);
	}
}