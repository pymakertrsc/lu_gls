#include"driverObjects.h"
#include "../../global/eventDispatcher.h"

namespace ff {

	DriverObjects::DriverObjects(
		const DriverGeometries::Ptr& geometries,
		const DriverAttributes::Ptr& attributes,
		const DriverInfo::Ptr& info
	) noexcept {
		mGeometries = geometries;
		mAttributes = attributes;
		mInfo = info;
	}

	DriverObjects::~DriverObjects() noexcept {}

	//����object��geometry֮update
	//��ͬ��object���ܻṲ��ͬһ��geometry
	//���������֤ÿ��geometryÿһ֡��ֻupdateһ��
	Geometry::Ptr DriverObjects::update(const RenderableObject::Ptr& object) noexcept {
		//1 �õ���ǰ���˵ڼ�֡
		const auto frame = mInfo->mRender.mFrame;

		//2 �ó�geometry��������get��������ص����ݼ�¼
		const auto geometry = mGeometries->get(object->getGeometry());
		
		//update once per frame,muti-objects-one geometry

		//key��geometry��ID
		//value��frameNumber
		auto iter = mUpdateMap.find(geometry->getID());

		//�����ǰframe��5
		//geometry3��һ�ν��룬geometry�ͻᱻDriverGeomtry��update
		//mUpdateMap����ͻ���루3��5��
		//����ڱ�֮֡�ڣ�����һ��objectҲʹ����geometry3
		//��ǰ3��geometry����Ӧ��frame��5���ͻ�ֱ�Ӳ�����

		//�����ʱ���뵽frame=6�����
		//geometry3�ٴα�����Ѱ�ң��ҵ���һ����ֵ�ԣ�3��5������ǰframe��5�����
		//geometry3�͵õ���һ��update�Ļ���,����mUpdateMap����ļ�ֵ�Ծͻᱻ���³ɣ�3��6��
		
		if(iter == mUpdateMap.end() || mUpdateMap[geometry->getID()] != frame) {
			mGeometries->update(geometry);
			mUpdateMap[geometry->getID()] = frame;
		}

		return geometry;
	}
}