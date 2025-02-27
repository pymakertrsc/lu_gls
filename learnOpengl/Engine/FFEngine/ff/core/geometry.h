#pragma once
#include "../global/base.h"
#include "attribute.h"
#include "../math/sphere.h"
#include "../math/box3.h"

namespace ff {

	//Geometry������ʾһ��mesh�Ļ����������ݣ����������Position Color Normal Uv Tangent Bitangent�ȵȵ�Attribute
	class Geometry:public std::enable_shared_from_this<Geometry> {
	public:

		//ΪAttribute��Map�ֵ����˸����� 
		using AttributeMap = std::unordered_map<std::string, Attributef::Ptr>;

		//����ָ�����&create����
		using Ptr = std::shared_ptr<Geometry>;
		static Ptr create() {
			return std::make_shared <Geometry>();
		}

		Geometry() noexcept;
		
		~Geometry() noexcept;
		
		void setAttribute(const std::string& name, Attributef::Ptr attribute) noexcept;

		Attributef::Ptr getAttribute(const std::string& name) noexcept;

		void setIndex(const Attributei::Ptr& index) noexcept;

		void deleteAttribute(const std::string& name) noexcept;

		bool hasAttribute(const std::string& name) noexcept;

		const AttributeMap& getAttributes() const noexcept;

		ID getID() const noexcept;

		auto getIndex() const noexcept { return mIndexAttribute; }

		void computeBoundingBox() noexcept;

		void computeBoundingSphere() noexcept;

		Sphere::Ptr getBoundingSphere() const noexcept { return mBoundingSphere; }
		Box3::Ptr getBoundingBox() const noexcept { return mBoundingBox; }
		
	protected:
		ID	mID{ 0 };//ȫ��Ψһid
		AttributeMap mAttributes{};//��������-ֵ�ķ�ʽ��������б�Mesh��Attributes��
		Attributei::Ptr mIndexAttribute{ nullptr };//index��Attribute������ţ���û�мӵ�map����

		Box3::Ptr	mBoundingBox{ nullptr };//��Χ��
		Sphere::Ptr	mBoundingSphere{ nullptr };//��Χ��
	};
}