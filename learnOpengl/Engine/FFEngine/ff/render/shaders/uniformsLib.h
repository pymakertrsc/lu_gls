#pragma once 
#include "../../global/base.h"
#include "../../global/constant.h"
#include "../../textures/texture.h"
#include "../../textures/cubeTexture.h"

namespace ff {

	//我们需要在绘制一个物体的时候，即做一次DrawCall的时候，就必须更新使用的Shader所必要的Uniforms，
	//将所需要更新的所有的Uniforms的数据，放在了统一的Map当中
	//本Map的key就是需要更新的Uniform的名字（跟Shader里面的那个Uniform的名字一一对应），对于PureArray与Single
	//对于StructuredUniforms，是按照层级的架构来选择名称
	//举例：(伪代码）
	// 比如除了基础矩阵之外，在shader当中，还存在如下结构体：
	// struct Test {
	//	float a;
	//  float b;
	// };
	// 
	// uniform Test t;
	// 
	// std::unordered_map<std::string, UniformHandle> outMap;
	// outMap["modelViewMatrix"] = UniformHandle(mValue = object->getModelViewMatrix())
	// outMap["normalMatrix"] = UniformHandle(mValue = object->getNormalMatrix())
	// 
	// std::unordered_map<std::string, std::any> contents;
	// contents["a"] = 0.0;
	// contents["b"] = 0.5;
	// 
	// UniformHandle::Ptr tHandle = UniformHandle{};
	// tHandle.mValue = contents;
	// 
	// outMap["t"] = tHandle;
	// 
	// 最终这个outMap就可以传入DriverUniforms的upload里面，进行更新
	// 
	// 再复杂一点：
	// struct Test {
	//	float a;
	//  float b;
	//	float ffs[10];
	// };
	// 
	// uniform Test t[2];
	// std::vector<float> myFFS0 = {1, 2 .... 10};
	// std::vector<float> myFFS1 = {10, 20 .... 100};
	//
	// 第一个t的数据，即t[0]
	// std::unordered_map<std::string, std::any> contents0;
	// contents0["a"] = 0.0;
	// contents0["b"] = 0.5;
	// contents0["ffs"] = myFFS0;
	// 
	// std::unordered_map<std::string, std::any> contents1;
	// contents1["a"] = 0.3;
	// contents1["b"] = 0.6; 
	// contents1["ffs"] = myFFS1;
	// 
	// std::unordered_map<std::string, std::any> contents;
	// contents["0"] = contents0;
	// contents["1"] = contents1;
	// 
	// UniformHandle::Ptr tHandle = UniformHandle{};
	// tHandle.mValue = contents;
	// 
	// outMap["t"] = tHandle;
	//
	// 如果是Pure Array Uniform如何更新
	// 
	// uniform float ffs[10];
	// 
	// ffs[0]
	// 
	// std::vector<float> myFFS = {1, 2 .... 10};
	// outMap["ffs"].mValue = myFFS;
	//

	class UniformHandle {
	public:
		UniformHandle() noexcept {}

		~UniformHandle() noexcept {}

		std::any	mValue;//可以是任何类型的值，比如float matrix map
		bool		mNeedsUpdate{ false };
	};

	using UniformUnitMap = std::unordered_map<std::string, std::any>;

	using UniformHandleMap = std::unordered_map<std::string, UniformHandle>;

	//跟ShaderChunk很像的UniformChunk
	static const std::unordered_map<std::string, UniformHandleMap> UniformsLib =
	{
		{
			"common", {
				{"diffuseMap", UniformHandle()},
				{"opacity", UniformHandle()}
			}
		},
		{
			"envMap", {
				{"envMap", UniformHandle()}
			}
		},
		{
			"specularMap", {
				{"specularMap", UniformHandle()}
			}
		},
		{
			"normalMap", {
				{"normalMap", UniformHandle()}
			}
		}

	};

	//------------tools-------------------------------
	static UniformHandleMap merge(const std::vector<UniformHandleMap>& uniformGroups) {
		UniformHandleMap mergedGroup;

		for (const auto& group : uniformGroups) {
			for (const auto& uniformIter : group) {
				auto name = uniformIter.first;
				auto handle = uniformIter.second;

				mergedGroup.insert(std::make_pair(name, handle));
			}
		}

		return std::move(mergedGroup);
	}

	//如果，我们在shader当中写的结构是：
	// struct Test {
	//	float a;
	//  float b;
	//	float ffs[10];
	// };
	// 
	// uniform Test t[2];
	// 
	// 比如光照的结构体，也可以做成一个数组
	// struct Light {
	// xxxx
	// }；
	// 
	// uniform Light lights[100];
	// 所以在外部构建outMap的时候，不可能一个个的去做赋值，然后一个个设置到contents，所以
	// 需要用一个循环，把所有的100个light结构体，都填充到contents这个Map里面
	// 
	// 在外部构建Outmap的时候，就得：
	// 。。。。各种操作。。。。
	// std::unordered_map<std::string, std::any> contents;
	// contents["0"] = contents0;
	// contents["1"] = contents1;
	//
	//函数的字面含义： 向array这个map当中，添加一个key为index的 uniformUnitMap
	//返回新添加的UniformUnitMap对象指针
	static UniformUnitMap* setArrayStructuredUniform(const uint32_t& index, UniformUnitMap* array) noexcept {
		auto indexString = std::to_string(index);
		(*array)[indexString] = UniformUnitMap();

		//将刚刚生成的UniformUnitMap对象，转换成UniformUnitMap*，cast成指针是由&来决定的
		return std::any_cast<UniformUnitMap>(&(*array)[indexString]);
	}

	//从array这个map当中，取得key为index的uniformUnitMap的指针
	static UniformUnitMap* getArrayStructuredUniform(const uint32_t& index, UniformUnitMap* array) noexcept {
		auto indexString = std::to_string(index);
		return std::any_cast<UniformUnitMap>(&(*array)[indexString]);
	}

	//工具函数，向array里面进行清理
	template<typename T>
	static void clearPureArrayUniform(std::vector<T>* array) {
		array->clear();
	}

	//工具函数，将value push到array
	template<typename T>
	static void pushPureArrayUniform(T value, std::vector<T>* array) {
		array->push_back(value);
	}

}
