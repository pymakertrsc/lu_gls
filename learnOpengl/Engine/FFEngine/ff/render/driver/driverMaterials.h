#pragma once
#include "../../global/base.h"
#include "../../global/constant.h"
#include "../../material/material.h"
#include "../../material/cubeMaterial.h"
#include "../../material/meshBasicMaterial.h"
#include "../../material/meshPhongMaterial.h"
#include "../../global/eventDispatcher.h"
#include "driverPrograms.h"
#include "driverUniforms.h"
#include "driverTextures.h"
#include "../shaders/uniformsLib.h"

namespace ff {

	//1 ��¼�˶�Ӧ��ǰ�˵�Material�Ĳ���
	//2 ��¼�˶�Ӧ��ǰ�˵�material����һ��ʼ����ǰ��ʹ�ù������е�DriverProgram
	class DriverMaterial {
	public:
		using Ptr = std::shared_ptr<DriverMaterial>;
		static Ptr create() { return std::make_shared<DriverMaterial>(); }

		DriverMaterial() noexcept;

		~DriverMaterial() noexcept;

	public:
		uint32_t				mVersion{ 0 };
		bool					mInstancing{ false };
		DriverProgram::Ptr		mCurrentProgram{ nullptr };

		Texture::Ptr			mDiffuseMap{ nullptr };
		Texture::Ptr			mEnvMap{ nullptr };
		Texture::Ptr			mNormalMap{ nullptr };
		Texture::Ptr			mSpecularMap{ nullptr };

		bool					mNeedsLight{ nullptr };
		uint32_t				mLightsStateVersion{ 0 };

		bool					mSkinning{ false };
		uint32_t				mMaxBones{ 0 };

		//��¼��ǰ�˶�Ӧ��material��ʹ�ù���driverPrograms
		//˼����
		// ������ǲ���¼��������ʹ�ù���DriverProgram��ֻ��¼��ǰ����ʹ�õ�Program
		// ��һ��material����֡��DiffuseMap�� ż��֡�ö���Color���ͻᵼ��DriverProgram���������ؽ����������ؽ�������
		//
		std::unordered_map<HashType, DriverProgram::Ptr> mPrograms{};//������ʱ�������ù��Ķ�--ref

		UniformHandleMap		mUniforms{};
	};

	class DriverMaterials {
	public:
		using Ptr = std::shared_ptr<DriverMaterials>;
		static Ptr create(const DriverPrograms::Ptr& programs) { return std::make_shared<DriverMaterials>(programs); }

		DriverMaterials(const DriverPrograms::Ptr& programs) noexcept;

		~DriverMaterials() noexcept;

		//����ǰ�˵�material�� ���غ�˶�Ӧ��DriverMaterial
		DriverMaterial::Ptr get(const Material::Ptr& material) noexcept;

		void onMaterialDispose(const EventBase::Ptr& event);

		//��������uniform����
		static void refreshMaterialUniforms(UniformHandleMap& uniformHandleMap, const Material::Ptr& material);

		static void refreshMaterialPhong(UniformHandleMap& uniformHandleMap, const MeshPhongMaterial::Ptr& material);

		static void refreshMaterialBasic(UniformHandleMap& uniformHandleMap, const MeshBasicMaterial::Ptr& material);

		static void refreshMaterialCube(UniformHandleMap& uniformHandleMap, const CubeMaterial::Ptr& material);

	private:
		DriverPrograms::Ptr mPrograms{ nullptr };

		//key-material id, value-driverMaterial
		std::unordered_map<ID, DriverMaterial::Ptr> mMaterials{};
	};
}