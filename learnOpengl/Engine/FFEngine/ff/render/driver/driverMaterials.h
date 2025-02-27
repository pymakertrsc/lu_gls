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

	//1 记录了对应的前端的Material的参数
	//2 记录了对应的前端的material，从一开始到当前，使用过的所有的DriverProgram
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

		//记录了前端对应的material所使用过的driverPrograms
		//思考：
		// 如果我们不记录所有曾经使用过的DriverProgram，只记录当前正在使用的Program
		// 当一个material奇数帧用DiffuseMap， 偶数帧用顶点Color，就会导致DriverProgram，析构，重建，析构，重建。。。
		//
		std::unordered_map<HashType, DriverProgram::Ptr> mPrograms{};//析构的时候连带用过的都--ref

		UniformHandleMap		mUniforms{};
	};

	class DriverMaterials {
	public:
		using Ptr = std::shared_ptr<DriverMaterials>;
		static Ptr create(const DriverPrograms::Ptr& programs) { return std::make_shared<DriverMaterials>(programs); }

		DriverMaterials(const DriverPrograms::Ptr& programs) noexcept;

		~DriverMaterials() noexcept;

		//传入前端的material， 返回后端对应的DriverMaterial
		DriverMaterial::Ptr get(const Material::Ptr& material) noexcept;

		void onMaterialDispose(const EventBase::Ptr& event);

		//用来更新uniform变量
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