#pragma once
#include "../../global/base.h"
#include "../../objects/renderableObject.h"
#include "../../material/material.h"
#include "driverUniforms.h"
#include "driverLights.h"
#include "driverShadowMap.h"
#include "../shaders/uniformsLib.h"

namespace ff {

	class DriverPrograms;
	class DriverProgram {
		friend DriverPrograms;
	public:

		struct Parameters {
			using Ptr = std::shared_ptr<Parameters>;
			static Ptr create() { return std::make_shared<Parameters>(); }

			std::string		mShaderID;//material 的Typename
			std::string		mVertex;//vs的代码
			std::string		mFragment;//fs的代码

			bool			mInstancing{ false };//是否启用实例绘制,Todo
			bool			mHasNormal{ false };//本次绘制的模型是否有法线
			bool			mHasUV{ false };//本次绘制的模型是否有uv
			bool			mHasColor{ false };//本次绘制的模型是否有顶点颜色
			bool			mHasDiffuseMap{ false };//本次绘制的模型所使用的材质是否有diffuseMap
			bool			mHasEnvCubeMap{ false };//本次绘制的模型所使用的材质是否有环境贴图
			bool			mHasSpecularMap{ false };//本次绘制的模型所使用的材质是否有镜面反射贴图

			bool			mShadowMapEnabled{ false };//是否启用阴影
			uint32_t		mDirectionalLightCount{ 0 };
			uint32_t		mNumDirectionalLightShadows{ 0 };

			bool			mUseTangent{ false };
			bool			mUseNormalMap{ false };

			bool			mSkinning{ false };
			uint32_t		mMaxBones{ 0 };

			uint32_t		mDepthPacking{ 0 };
		};

		using Ptr = std::shared_ptr<DriverProgram>;
		static Ptr create(const Parameters::Ptr& parameters) {
			return std::make_shared <DriverProgram>(parameters);
		}

		DriverProgram(const Parameters::Ptr& parameters) noexcept;

		~DriverProgram() noexcept;

		auto getID() const noexcept { return mID; }

		auto getCacheKey() const noexcept { return mCacheKey; }

		GLuint		mProgram{ 0 };

		void uploadUniforms(UniformHandleMap& uniformGroup, const DriverTextures::Ptr& textures);

	private:
		void replaceAttributeLocations(std::string& shader) noexcept;
		void replaceLightNumbers(std::string& shader, const Parameters::Ptr& parameters) noexcept;

		std::string getExtensionString() noexcept;

	private:
		uint32_t	mID{ 0 };//driverProgram 自己的id号
		HashType	mCacheKey{ 0 };//由parameters参数合集计算出来的hash值
		uint32_t	mRefCount{ 0 };//控制外界有多少引用本Program的renderItem
		DriverUniforms::Ptr mUniforms = nullptr;
	};

	//1 对于DriverProgram的管理,存储成了一个map，key是program的哈希值，value就是DriverProgram的智能指针
	//2 对外暴露辅助逻辑进行的接口
	class DriverPrograms {
	public:
		using Ptr = std::shared_ptr<DriverPrograms>;
		static Ptr create() {
			return std::make_shared <DriverPrograms>();
		}

		DriverPrograms() noexcept;

		~DriverPrograms() noexcept;

		DriverProgram::Ptr acquireProgram(const DriverProgram::Parameters::Ptr& parameters, HashType cacheKey) noexcept;

		UniformHandleMap getUniforms(const Material::Ptr& material) noexcept;

		//传入当前渲染物体的材质、object3D、光源信息、阴影信息，从这些东西里面
		//提取创建shader所必要的信息，组成一个parameters返回
		DriverProgram::Parameters::Ptr getParameters(
			const Material::Ptr& material,
			const Object3D::Ptr& object, 
			const DriverLights::Ptr& lights,
			const DriverShadowMap::Ptr& shadowMap) noexcept;

		HashType getProgramCacheKey(const DriverProgram::Parameters::Ptr& parameters) noexcept;

		void release(const DriverProgram::Ptr& program) noexcept;

	private:
		//key-paramters做成的哈希值，value-用本parameters生成的driverProgram
		std::unordered_map<HashType, DriverProgram::Ptr> mPrograms{};
	};
}