#pragma once
#include "../global/base.h"

namespace ff {

	class DebugLog {
	public:
		static DebugLog* getInstance();
		bool mEnableDebug{ false };

		~DebugLog() noexcept;

		void end() noexcept;

		//print when parse uniform
		void beginPrintUniformInfo(std::string shaderID) noexcept;
		void printUniformInfo(int index, GLsizei length, GLint size, GLenum type, GLchar* name) noexcept;

		//
		void beginUpLoad(std::string materialType) noexcept;

		void printMatrix(const glm::mat4& matrix) noexcept;

	private:
		DebugLog() noexcept;

		std::string toString(GLenum type) noexcept;

	private:
		static DebugLog* mInstance;
	};
}