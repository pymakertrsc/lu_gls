#pragma once 
#include "../../global/base.h"
#include "../../global/constant.h"
#include "../../textures/texture.h"
#include "../../textures/cubeTexture.h"

namespace ff {

	//������Ҫ�ڻ���һ�������ʱ�򣬼���һ��DrawCall��ʱ�򣬾ͱ������ʹ�õ�Shader����Ҫ��Uniforms��
	//������Ҫ���µ����е�Uniforms�����ݣ�������ͳһ��Map����
	//��Map��key������Ҫ���µ�Uniform�����֣���Shader������Ǹ�Uniform������һһ��Ӧ��������PureArray��Single
	//����StructuredUniforms���ǰ��ղ㼶�ļܹ���ѡ������
	//������(α���룩
	// ������˻�������֮�⣬��shader���У����������½ṹ�壺
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
	// �������outMap�Ϳ��Դ���DriverUniforms��upload���棬���и���
	// 
	// �ٸ���һ�㣺
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
	// ��һ��t�����ݣ���t[0]
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
	// �����Pure Array Uniform��θ���
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

		std::any	mValue;//�������κ����͵�ֵ������float matrix map
		bool		mNeedsUpdate{ false };
	};

	using UniformUnitMap = std::unordered_map<std::string, std::any>;

	using UniformHandleMap = std::unordered_map<std::string, UniformHandle>;

	//��ShaderChunk�����UniformChunk
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

	//�����������shader����д�Ľṹ�ǣ�
	// struct Test {
	//	float a;
	//  float b;
	//	float ffs[10];
	// };
	// 
	// uniform Test t[2];
	// 
	// ������յĽṹ�壬Ҳ��������һ������
	// struct Light {
	// xxxx
	// }��
	// 
	// uniform Light lights[100];
	// �������ⲿ����outMap��ʱ�򣬲�����һ������ȥ����ֵ��Ȼ��һ�������õ�contents������
	// ��Ҫ��һ��ѭ���������е�100��light�ṹ�壬����䵽contents���Map����
	// 
	// ���ⲿ����Outmap��ʱ�򣬾͵ã�
	// �����������ֲ�����������
	// std::unordered_map<std::string, std::any> contents;
	// contents["0"] = contents0;
	// contents["1"] = contents1;
	//
	//���������溬�壺 ��array���map���У����һ��keyΪindex�� uniformUnitMap
	//��������ӵ�UniformUnitMap����ָ��
	static UniformUnitMap* setArrayStructuredUniform(const uint32_t& index, UniformUnitMap* array) noexcept {
		auto indexString = std::to_string(index);
		(*array)[indexString] = UniformUnitMap();

		//���ո����ɵ�UniformUnitMap����ת����UniformUnitMap*��cast��ָ������&��������
		return std::any_cast<UniformUnitMap>(&(*array)[indexString]);
	}

	//��array���map���У�ȡ��keyΪindex��uniformUnitMap��ָ��
	static UniformUnitMap* getArrayStructuredUniform(const uint32_t& index, UniformUnitMap* array) noexcept {
		auto indexString = std::to_string(index);
		return std::any_cast<UniformUnitMap>(&(*array)[indexString]);
	}

	//���ߺ�������array�����������
	template<typename T>
	static void clearPureArrayUniform(std::vector<T>* array) {
		array->clear();
	}

	//���ߺ�������value push��array
	template<typename T>
	static void pushPureArrayUniform(T value, std::vector<T>* array) {
		array->push_back(value);
	}

}
