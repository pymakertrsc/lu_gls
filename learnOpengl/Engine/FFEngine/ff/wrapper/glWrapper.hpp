#pragma once
#include "../global/base.h"

namespace ff {
	namespace gl {
		static void checkError() {
			GLenum errorCode;
			bool needsAssert = false;
			std::string error;
			while ((errorCode = glGetError()) != GL_NO_ERROR)
			{
				needsAssert = true;
				switch (errorCode)
				{
				case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
				case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
				case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
				case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
				case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
				case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
				case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
				}
				std::cout << error << std::endl;
			}
			assert(!needsAssert);
		}

		//state
		static void enable(GLenum cap) {
			glEnable(cap);
			checkError();
		}

		static void disable(GLenum cap) {
			glDisable(cap);
			checkError();
		}

		static void clearColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
			glClearColor(r, g, b, a);
			checkError();
		}

		static void uniform1iv(GLint location, GLsizei size, GLint* value) {
			glUniform1iv(location, size, value);
			checkError();
		}

		static void uniform1i(GLint location, GLint value) {
			glUniform1i(location, value);
			checkError();
		}


		//blend
		static void blendFuncSeparate(
			GLenum srcRGB,
			GLenum dstRGB,
			GLenum srcAlpha,
			GLenum dstAlpha) {

			glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
			checkError();
		}

		static void blendEquationSeparate(GLenum modeRGB, GLenum modeAlpha) {
			glBlendEquationSeparate(modeRGB, modeAlpha);
			checkError();
		}

		//depth
		static void depthMask(GLboolean flag) {
			glDepthMask(flag);
			checkError();
		}

		static void depthFunc(GLenum func) {
			glDepthFunc(func);
			checkError();
		}

		static void clearDepth(GLdouble depth) {
			glClearDepth(depth);
			checkError();
		}
	}
	
}
