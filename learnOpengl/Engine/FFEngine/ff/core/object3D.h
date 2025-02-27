#pragma once
#include "../global/base.h"

namespace ff {

	class ObjectTypeChecker {
	public:
		bool mIsRenderableObject{ false };
		bool mIsMesh{ false };
		bool mIsSkinnedMesh{ false };
		bool mIsBone{ false };
		bool mIsScene{ false };
		bool mIsCamera{ false };
		bool mIsPerpectiveCamera{ false };
		bool mIsOrthographicCamera{ false };
		bool mIsGroup{ false };
		bool mIsLight{ false };
		bool mIsAmbientLight{ false };
		bool mIsDirectionalLight{ false };
	};

	//Object3D�����пռ�任���ڵ�ṹ�ȵ������������
	class Object3D : public std::enable_shared_from_this<Object3D>, public ObjectTypeChecker {
	public:
		//ʹ������ָ����й���
		using Ptr = std::shared_ptr<Object3D>;
		static Ptr create() {
			return std::make_shared <Object3D>();
		}

		Object3D() noexcept;

		~Object3D() noexcept;

		//���㶯̬����ת��
		//Parent* p = new ChildA();
		//ChildA* c = p;���У���Ϊparent������������ChildA ChildB������
		//ChildA* c = dynamic_cast<ChildA>(p);//static_cast reinterpret_cast 
		//dynamic��̬ת����������ͼ��飬���p������ChildA���ֶ�����ôʧ�ܣ�����nullptr
		//dynamicҪ����������麯��
		//����ָ��Ҳӵ��һ������ת����ϵ�����£�
		/*std::reinterpret_pointer_cast
		std::dynamic_pointer_cast;
		std::static_pointer_cast;*/
		virtual void fakeFunction() noexcept {}

		void setPosition(float x, float y, float z) noexcept;

		void setPosition(const glm::vec3& position) noexcept;

		void setQuaternion(float x, float y, float z, float w) noexcept;

		void setScale(float x, float y, float z) noexcept;

		void rotateX(float angle) noexcept;

		void rotateY(float angle) noexcept;

		void rotateZ(float angle) noexcept;

		void setRotateAroundAxis(const glm::vec3& axis, float angle) noexcept;

		void rotateAroundAxis(const glm::vec3& axis, float angle) noexcept;

		void lookAt(const glm::vec3& target, const glm::vec3& up) noexcept;

		void setLocalMatrix(const glm::mat4& localMatrix) noexcept;

		void setWorldMatrix(const glm::mat4& worldMatrix) noexcept;

		//��ǰ��Object3D���棬�����ӽڵ�
		void addChild(const Object3D::Ptr& child) noexcept;

		virtual void updateMatrix() noexcept;

		virtual glm::mat4 updateWorldMatrix(bool updateParent = false, bool updateChildren = false) noexcept;

		glm::mat4 updateModelViewMatrix(const glm::mat4& viewMatrix) noexcept;

		glm::mat3 updateNormalMatrix() noexcept;

		glm::vec3 getPosition() const noexcept;

		glm::vec3 getWorldPosition() const noexcept;

		glm::vec3 getLocalDirection() const noexcept;

		glm::vec3 getWorldDirection() const noexcept;

		glm::vec3 getUp() const noexcept;

		glm::vec3 getRight() const noexcept;

		glm::mat4 getLocalMatrix() noexcept;

		glm::mat4 getWorldMatrix() noexcept;

		glm::mat4 getModelViewMatrix() noexcept;

		glm::mat3 getNormalMatrix() noexcept;

		const std::vector<Object3D::Ptr>& getChildren() const noexcept;

		ID getID() const noexcept;

	protected:
		void decompose() noexcept;

	public:
		//visible����ʾ�Ƿ���������Ⱦ
		bool	mVisible{ true };

		//�Ƿ������Ӱ
		bool	mCastShadow{ true };

		//obj������
		std::string mName;

		//��ʾ�Ƿ�ǿ�ƶԾ�����и���
		bool	mNeedsUpdateMatrix{ true };

	protected:
		ID	mID{ 0 };//ȫ��Ψһid

		//for setting easy
		glm::vec3 mPosition{ glm::vec3(0.0f) };

		//����object����ת�任������һ�ɲ�����Ԫ����������Ԫ������ϸ�Ƶ�����������
		//��qq��396198587  wechat��carmazzz
		glm::quat mQuaternion{ glm::quat(1.0f, 0.0f, 0.0f, 0.0f) };

		glm::vec3 mScale{ glm::vec3(1.0f) };

		//localMatrix����ģ������ϵ�����˱任
		glm::mat4	mLocalMatrix = glm::mat4(1.0f);

		//worldMatrix��ģ�Ͷ����ģ������ϵ��ת������������ϵ
		glm::mat4	mWorldMatrix = glm::mat4(1.0f);

		//��������
		bool		mNeedsUpdate{ false };

		//�ڵ�ϵͳ
		//���ڵ����weakPtr ����ֹѭ������
		std::weak_ptr<Object3D>		mParent;
		//���ڵ�洢���ӽڵ��sharedPtr������һ������,��֤�ӽڵ�����ü������ٴ���1
		std::vector<Object3D::Ptr>	mChildren{};

		//for shading
		//modelViewMatrix��ģ�Ͷ��㣬��ģ������ϵ��ת������ǰ���������ϵ,viewMatrix * worldMatrix
		glm::mat4	mModelViewMatrix = glm::mat4(1.0f);

		//��ģ�͵�normal��ģ������ϵ��ת�������������ϵ
		glm::mat3	mNormalMatrix = glm::mat3(1.0f);
	};
}