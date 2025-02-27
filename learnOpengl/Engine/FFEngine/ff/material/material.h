#pragma once
#include "../global/base.h"
#include "../global/constant.h"
#include "../textures/texture.h"
#include "../textures/cubeTexture.h"

namespace ff {
	//材质类，描述一个mesh绘制的时候，用什么贴图，走什么样的管线，过什么样的shader
	//根据材质类型不同，可能会有一些特殊参数
	//多个mesh可以应用同一个材质Material Ptr
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
		//使用智能指针管理
		using Ptr = std::shared_ptr<Material>;
		static Ptr create() {
			return std::make_shared <Material>();
		}

		Material() noexcept;

		~Material() noexcept;

	public:
		std::string getType() const noexcept { return mType; }
		ID getID() const noexcept { return mID; }

		//表示再backend里面，是否需要更新材质参数
		bool		mNeedsUpdate{ true };

		//version 用于首次解析
		uint32_t	mVersion{ 1 };

		//raster
		FrontFace	mFrontFace{ FrontFace::FrontCounterClockWise };
		Side		mSide{ Side::DoubleSide };
		//现在要绘制的mesh，是使用三角形啊 线啊
		DrawMode	mDrawMode{ DrawMode::Triangles };

		//blending
		bool				mTransparent{ false };//控制是否开启blending
		float				mOpacity{ 1.0f };//透明度，越小，越透明
		BlendingType		mBlendingType{ BlendingType::CustomBlending };
		BlendingFactor		mBlendSrc{ BlendingFactor::SrcAlpha };
		BlendingFactor		mBlendDst{ BlendingFactor::OneMinusSrcAlpha };
		BlendingEquation	mBlendEquation{ BlendingEquation::AddEquation };
		BlendingFactor		mBlendSrcAlpha{ BlendingFactor::None };
		BlendingFactor		mBlendDstAlpha{ BlendingFactor::Zero };
		BlendingEquation	mBlendEquationAlpha{ BlendingEquation::AddEquation };
		
		//depth
		bool mDepthTest{ true };//是否开启深度检测
		bool mDepthWrite{ true };//是否在深度检测通过后，用本物体的深度覆盖原来的深度
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