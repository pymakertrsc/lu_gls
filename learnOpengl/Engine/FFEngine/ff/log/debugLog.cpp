#include "debugLog.h"

namespace ff {
	
	DebugLog* DebugLog::mInstance = nullptr;
	DebugLog* DebugLog::getInstance() {
		if (mInstance == nullptr) {
			mInstance = new DebugLog();
		}

		return mInstance;
	}

	DebugLog::DebugLog() noexcept {}

	DebugLog::~DebugLog() noexcept {}

	void DebugLog::beginPrintUniformInfo(std::string shaderID) noexcept {
		if (!mEnableDebug) {
			return;
		}

		std::cout << "-----------Shader Uniform Info--------------" << std::endl;
		std::cout << "ShaderID: " << shaderID << std::endl;
		std::cout << std::endl;
	}
	void DebugLog::printUniformInfo(int index, GLsizei length, GLint size, GLenum type, GLchar* name) noexcept {
		if (!mEnableDebug) {
			return;
		}

		std::cout << "Location:" << index << std::endl;
		std::cout << "Name Length:" << length << std::endl;
		std::cout << "Uniform Size:" << size << std::endl;
		std::cout << "Uniform Type:" << toString(type) << std::endl;
		std::cout << "Uniform Name:" << name << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;
	}
	void DebugLog::end() noexcept {
		if (!mEnableDebug) {
			return;
		}

		std::cout << "-----------E  N  D--------------" << std::endl;
		std::cout << std::endl;
	}

	void DebugLog::beginUpLoad(std::string materialType) noexcept {
		if (!mEnableDebug) {
			return;
		}

		std::cout << "-----------BEGIN UPLOAD UNIFORM--------------" << std::endl;
		std::cout << "MaterialType: " << materialType << std::endl;
		std::cout << std::endl;
	}

	void DebugLog::printMatrix(const glm::mat4& matrix) noexcept {
		std::cout << "Matrix:" << std::endl;
		std::cout << matrix[0][0] << "," << matrix[0][1] << "," << matrix[0][2] << "," << matrix[0][3] << std::endl;
		std::cout << matrix[1][0] << "," << matrix[1][1] << "," << matrix[1][2] << "," << matrix[1][3] << std::endl;
		std::cout << matrix[2][0] << "," << matrix[2][1] << "," << matrix[2][2] << "," << matrix[2][3] << std::endl;
		std::cout << matrix[3][0] << "," << matrix[3][1] << "," << matrix[3][2] << "," << matrix[3][3] << std::endl;
	}


	
	std::string DebugLog::toString(GLenum type) noexcept {
	
		std::string result;
		switch (type) {
		case GL_FLOAT:
			result = "float";
				break;
		case GL_FLOAT_VEC2:
			result = "float vec2";
				break;
		case GL_FLOAT_VEC3:
			result = "float vec3";
				break;
		case GL_FLOAT_VEC4:
			result = "float vec4";
				break;
		case GL_INT:
			result = "int";
				break;
		case GL_INT_VEC2:
			result = "int vec2";
				break;
		case GL_INT_VEC3:
			result = "int vec3";
				break;
		case GL_INT_VEC4:
			result = "int vec4";
				break;
		case GL_BOOL:
			result = "bool";
				break;
		case GL_BOOL_VEC2:
			result = "bool vec2";
				break;
		case GL_BOOL_VEC3:
			result = "bool vec3";
				break;
		case GL_BOOL_VEC4:
			result = "bool vec4";
				break;
		case GL_FLOAT_MAT2:
			result = "mat2";
				break;
		case GL_FLOAT_MAT3:
			result = "mat3";
				break;
		case GL_FLOAT_MAT4:
			result = "mat4";
				break;
		case GL_SAMPLER_2D:
			result = "sampler 2D";
			break;
		case GL_SAMPLER_CUBE:
			result = "sampler Cube";
			break;
		default:
			break;
		}

		return result;
	}
}