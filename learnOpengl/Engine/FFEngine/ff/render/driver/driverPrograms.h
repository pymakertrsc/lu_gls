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

			std::string		mShaderID;//material ��Typename
			std::string		mVertex;//vs�Ĵ���
			std::string		mFragment;//fs�Ĵ���

			bool			mInstancing{ false };//�Ƿ�����ʵ������,Todo
			bool			mHasNormal{ false };//���λ��Ƶ�ģ���Ƿ��з���
			bool			mHasUV{ false };//���λ��Ƶ�ģ���Ƿ���uv
			bool			mHasColor{ false };//���λ��Ƶ�ģ���Ƿ��ж�����ɫ
			bool			mHasDiffuseMap{ false };//���λ��Ƶ�ģ����ʹ�õĲ����Ƿ���diffuseMap
			bool			mHasEnvCubeMap{ false };//���λ��Ƶ�ģ����ʹ�õĲ����Ƿ��л�����ͼ
			bool			mHasSpecularMap{ false };//���λ��Ƶ�ģ����ʹ�õĲ����Ƿ��о��淴����ͼ

			bool			mShadowMapEnabled{ false };//�Ƿ�������Ӱ
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
		uint32_t	mID{ 0 };//driverProgram �Լ���id��
		HashType	mCacheKey{ 0 };//��parameters�����ϼ����������hashֵ
		uint32_t	mRefCount{ 0 };//��������ж������ñ�Program��renderItem
		DriverUniforms::Ptr mUniforms = nullptr;
	};

	//1 ����DriverProgram�Ĺ���,�洢����һ��map��key��program�Ĺ�ϣֵ��value����DriverProgram������ָ��
	//2 ���Ⱪ¶�����߼����еĽӿ�
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

		//���뵱ǰ��Ⱦ����Ĳ��ʡ�object3D����Դ��Ϣ����Ӱ��Ϣ������Щ��������
		//��ȡ����shader����Ҫ����Ϣ�����һ��parameters����
		DriverProgram::Parameters::Ptr getParameters(
			const Material::Ptr& material,
			const Object3D::Ptr& object, 
			const DriverLights::Ptr& lights,
			const DriverShadowMap::Ptr& shadowMap) noexcept;

		HashType getProgramCacheKey(const DriverProgram::Parameters::Ptr& parameters) noexcept;

		void release(const DriverProgram::Ptr& program) noexcept;

	private:
		//key-paramters���ɵĹ�ϣֵ��value-�ñ�parameters���ɵ�driverProgram
		std::unordered_map<HashType, DriverProgram::Ptr> mPrograms{};
	};
}