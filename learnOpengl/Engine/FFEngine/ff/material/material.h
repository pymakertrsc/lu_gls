#pragma once
#include "../global/base.h"
#include "../global/constant.h"
#include "../textures/texture.h"
#include "../textures/cubeTexture.h"

namespace ff {
	//�����࣬����һ��mesh���Ƶ�ʱ����ʲô��ͼ����ʲô���Ĺ��ߣ���ʲô����shader
	//���ݲ������Ͳ�ͬ�����ܻ���һЩ�������
	//���mesh����Ӧ��ͬһ������Material Ptr
	class MaterialTypeChecker {
	public:
		bool mIsMaterial = false;
		bool mIsMeshBasicMaterial = false;
		bool mIsMeshPhongMaterial = false;
		bool mIsLineBasicMaterial = false;
		bool mIsCubeMaterial = false;
		bool mIsDepthMaterial = false;
	};

	class Material:public MaterialTypeChecker {
	public:
		//ʹ������ָ�����
		using Ptr = std::shared_ptr<Material>;
		static Ptr create() {
			return std::make_shared <Material>();
		}

		Material() noexcept;

		~Material() noexcept;

	public:
		std::string getType() const noexcept { return mType; }
		ID getID() const noexcept { return mID; }

		//��ʾ��backend���棬�Ƿ���Ҫ���²��ʲ���
		bool		mNeedsUpdate{ true };

		//version �����״ν���
		uint32_t	mVersion{ 1 };

		//raster
		FrontFace	mFrontFace{ FrontFace::FrontCounterClockWise };
		Side		mSide{ Side::DoubleSide };
		//����Ҫ���Ƶ�mesh����ʹ�������ΰ� �߰�
		DrawMode	mDrawMode{ DrawMode::Triangles };

		//blending
		bool				mTransparent{ false };//�����Ƿ���blending
		float				mOpacity{ 1.0f };//͸���ȣ�ԽС��Խ͸��
		BlendingType		mBlendingType{ BlendingType::CustomBlending };
		BlendingFactor		mBlendSrc{ BlendingFactor::SrcAlpha };
		BlendingFactor		mBlendDst{ BlendingFactor::OneMinusSrcAlpha };
		BlendingEquation	mBlendEquation{ BlendingEquation::AddEquation };
		BlendingFactor		mBlendSrcAlpha{ BlendingFactor::None };
		BlendingFactor		mBlendDstAlpha{ BlendingFactor::Zero };
		BlendingEquation	mBlendEquationAlpha{ BlendingEquation::AddEquation };
		
		//depth
		bool mDepthTest{ true };//�Ƿ�����ȼ��
		bool mDepthWrite{ true };//�Ƿ�����ȼ��ͨ�����ñ��������ȸ���ԭ�������
		CompareFunction mDepthFunction{ CompareFunction::LessOrEqual };
		double mDepthClearColor{ 1.0 };

		//diffuse
		Texture::Ptr	mDiffuseMap{ nullptr };

		//envMap
		CubeTexture::Ptr	mEnvMap{ nullptr };

		//normalMap
		Texture::Ptr	mNormalMap{ nullptr };

		//specularMap
		Texture::Ptr	mSpecularMap{ nullptr };

	protected:
		ID mID{ 0 };
		std::string mType;
	};
}