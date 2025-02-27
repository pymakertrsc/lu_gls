#pragma once
#include "../../global/base.h"
#include "../../objects/renderableObject.h"
#include "../../core/geometry.h"
#include "../../material/material.h"

namespace ff {

	//mesh line skinnedMesh ���ᱻ����ΪRenderItem
	class RenderItem {
	public:
		using Ptr = std::shared_ptr<RenderItem>;
		static Ptr create() {
			return std::make_shared <RenderItem>();
		}

		RenderItem() noexcept;

		~RenderItem() noexcept;

	public:
		ID					mID{ 0 };
		float				mZ = 0;//��������-��Ⱦ͸�������ʱ���Ǵ�Զ����������Ⱦ
		RenderableObject::Ptr	mObject{ nullptr };
		Material::Ptr		mMaterial{ nullptr };
		Geometry::Ptr		mGeometry{ nullptr };
		uint32_t			mGroupOrder{ 0 };//Ӱ����Ⱦ˳��
	};

	using RenderListSortFunction = std::function<bool(const RenderItem::Ptr&, const RenderItem::Ptr&)>;

	//�����ԭ��:
	//1 opaque������Ҫ����
	// 
	//>��ʾ�����ǰ��
	static bool  smallerZFirstSort(const RenderItem::Ptr& item0, const RenderItem::Ptr& item1) {
		//���ȱ�֤groupOrder��������Ȼ���
		if (item0->mGroupOrder != item1->mGroupOrder) {
			return  item0->mGroupOrder > item1->mGroupOrder;
		}
		//С��z������ǰ��
		else if(item0->mZ != item1->mZ) {
			return item0->mZ < item1->mZ;
		}
		else {
			//���groupOrder��z�ֱ����,����sort����������Ҫ������һ��true or false
			//idԽ��˵��������Խ���򴴽�Խ������壬Խ�Ȼ���
			return item0->mID > item1->mID;
		}
	}

	static bool  biggerZFirstSort(const RenderItem::Ptr& item0, const RenderItem::Ptr& item1) {
		if (item0->mGroupOrder != item1->mGroupOrder) {
			return  item0->mGroupOrder > item1->mGroupOrder;
		}
		else if (item0->mZ != item1->mZ) {
			//zԽ������Խ��ǰ
			return item0->mZ > item1->mZ;
		}
		else {
			return item0->mID > item1->mID;
		}
	}

	//driverRenderList�����洢����������Ⱦ��Ԫ
	class DriverRenderList {
	public:
		using Ptr = std::shared_ptr<DriverRenderList>;
		static Ptr create() {
			return std::make_shared <DriverRenderList>();
		}

		DriverRenderList();

		~DriverRenderList();

		//ÿһ֡��ʼ��render��ʱ�򣬶���Ҫͨ��init����ʼ����Ⱦ�б�
		void init() noexcept;

		//����Ⱦ�б��м���һ��renderItem
		void push(
			const RenderableObject::Ptr& object,
			const Geometry::Ptr& geometry,
			const Material::Ptr& material,
			const uint32_t&	groupOrder,
			float z) noexcept;

		//�������,����������ڷ�͸�������Լ�͸����������������
		void sort(
			const RenderListSortFunction& opaqueSort = smallerZFirstSort, //Ϊ��earlyz
			const RenderListSortFunction& transparentSort = biggerZFirstSort) noexcept;//Ϊ����ɫ�����ȷ

		//��ÿһ�ι��������Ⱦ�б��ʱ�򣬵���finish
		void finish() noexcept;

		const auto& getOpaques() const noexcept { return mOpaques; }

		const auto& getTransparents() const noexcept { return mTransparents; }

	private:
		//ÿһ��pushһ������Ⱦ���壬������ñ���������������������renderItem����
		//��cache�����ȡһ�����õģ����᷵��һ�����õ�renderItem
		RenderItem::Ptr getNextRenderItem(
			const RenderableObject::Ptr& object,
			const Geometry::Ptr& geometry,
			const Material::Ptr& material,
			const uint32_t& groupOrder,
			float z) noexcept;

	private:
		//����������ǰ��Ⱦ�б��item������ÿһ֡��ʼʱ����init���涼�ᱻ��Ϊ0
		uint32_t mRenderItemIndex{ 0 };

		std::vector<RenderItem::Ptr> mOpaques{};//�洢��͸�����������ָ��
		std::vector<RenderItem::Ptr> mTransparents{};//�洢͸�����������ָ��

		std::vector<RenderItem::Ptr> mRenderItemCache{};//����renderItem
	};
}