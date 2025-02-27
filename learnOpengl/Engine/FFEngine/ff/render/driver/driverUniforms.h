#pragma once
#include "../../global/base.h"
#include "../../global/constant.h"
#include "driverTextures.h"
#include "../shaders/uniformsLib.h"

namespace ff {

	class DriverUniforms;

	//一切uniform类型的根类
	class UniformBase {
	public:
		using Ptr = std::shared_ptr<UniformBase>;

		UniformBase() noexcept;

		~UniformBase() noexcept;

		//uniform的名字
		std::string mID;

		//完成更新uniform这件事儿
		virtual void setValue(
			const std::any& value,
			const DriverTextures::Ptr& textures,
			const std::shared_ptr<DriverUniforms>& driverUniforms) {}
	};

	//1 存储location type类型
	//2 提供setValue的接口，并且在其内部，根据不同类型的uniform调用不同的upload

	class SingleUniform : public UniformBase {
	public:
		using Ptr = std::shared_ptr<SingleUniform>;
		static Ptr create(const std::string& id, const GLint& location, const GLenum& type) {
			return std::make_shared<SingleUniform>(id, location, type);
		}

		SingleUniform(const std::string& id, const GLint& location, const GLenum& type) noexcept;

		~SingleUniform() noexcept;

	public:
		GLint		mLocation{ 0 };
		GLenum		mType;

	public:

		void setValue(
			const std::any& value,
			const DriverTextures::Ptr& textures,
			const std::shared_ptr<DriverUniforms>& driverUniforms) override;

		void uploadTexture(
			const std::shared_ptr<DriverUniforms>& driverUniforms,
			const DriverTextures::Ptr& textures,
			const std::any& value);

		//将T类型的变量value，传输到location所代表的uniform里面
		template<typename T>
		void upload(const T& value) {}

		//模板偏特化
		template<>
		void upload<float>(const float& value);

		template<>
		void upload<glm::vec2>(const glm::vec2& value);

		template<>
		void upload<glm::vec3>(const glm::vec3& value);

		template<>
		void upload<glm::vec4>(const glm::vec4& value);

		template<>
		void upload<int>(const int& value);

		template<>
		void upload<glm::ivec2>(const glm::ivec2& value);

		template<>
		void upload<glm::ivec3>(const glm::ivec3& value);

		template<>
		void upload<glm::ivec4>(const glm::ivec4& value);

		template<>
		void upload<bool>(const bool& value);

		template<>
		void upload<glm::bvec2>(const glm::bvec2& value);

		template<>
		void upload<glm::bvec3>(const glm::bvec3& value);

		template<>
		void upload<glm::bvec4>(const glm::bvec4& value);

		template<>
		void upload<glm::mat2>(const glm::mat2& value);

		template<>
		void upload<glm::mat3>(const glm::mat3& value);

		template<>
		void upload<glm::mat4>(const glm::mat4& value);

	};

	//纯粹的数组类型的Uniform
	class PureArrayUniform : public UniformBase {
	public:
		using Ptr = std::shared_ptr<PureArrayUniform>;
		using Ptr = std::shared_ptr<PureArrayUniform>;
		static Ptr create(const std::string& id, const GLint& location, const GLenum& type, GLint size) {
			return std::make_shared<PureArrayUniform>(id, location, type, size);
		}

		PureArrayUniform(const std::string& id, const GLint& location, const GLenum& type, GLint size) noexcept;

		~PureArrayUniform() noexcept;

		GLint		mLocation{ 0 };
		GLenum		mType;
		GLint		mSize{ 0 };

	public:
		void setValue(
			const std::any& value,
			const DriverTextures::Ptr& textures,
			const std::shared_ptr<DriverUniforms>& driverUniforms) override;

		void uploadTexture2DArray(
			const std::shared_ptr<DriverUniforms>& driverUniforms,
			const DriverTextures::Ptr& textures,
			const std::any& value);

		template<typename T>
		void upload(const T* value) {}

		template<>
		void upload<float>(const float* value);

		template<>
		void upload<glm::vec2>(const glm::vec2* value);

		template<>
		void upload<glm::vec3>(const glm::vec3* value);

		template<>
		void upload<glm::vec4>(const glm::vec4* value);

		template<>
		void upload<int>(const int* value);

		template<>
		void upload<glm::ivec2>(const glm::ivec2* value);

		template<>
		void upload<glm::ivec3>(const glm::ivec3* value);

		template<>
		void upload<glm::ivec4>(const glm::ivec4* value);

		//no bool bvec2 bvec3 bvec4 we use int instead

		template<>
		void upload<glm::mat2>(const glm::mat2* value);

		template<>
		void upload<glm::mat3>(const glm::mat3* value);

		template<>
		void upload<glm::mat4>(const glm::mat4* value);

	};

	class UniformContainer {
	public:
		using Ptr = std::shared_ptr<UniformContainer>;
		std::unordered_map<std::string, UniformBase::Ptr> mUniformMap;

		virtual void fake() {}
	};

	class StructuredUniform :public UniformBase, public UniformContainer {
	public:
		using Ptr = std::shared_ptr<StructuredUniform>;
		static Ptr create(const std::string& id) {
			return std::make_shared<StructuredUniform>(id);
		}

		StructuredUniform(const std::string& id) noexcept;

		~StructuredUniform() noexcept;

		void setValue(
			const std::any& value,
			const DriverTextures::Ptr& textures,
			const std::shared_ptr<DriverUniforms>& driverUniforms) override;
	};

	class DriverUniforms :public UniformContainer, public std::enable_shared_from_this<DriverUniforms> {
	public:
		using Ptr = std::shared_ptr<DriverUniforms>;
		static Ptr create(const GLint& program) { return std::make_shared<DriverUniforms>(program); }

		DriverUniforms(const GLint& program) noexcept;

		~DriverUniforms() noexcept;

		void upload(UniformHandleMap& uniformHandleMap, const DriverTextures::Ptr& textures);

		void addUniform(UniformContainer* container, const UniformBase::Ptr& uniformObject);

		//texture slots
		void setTextureSlot(const GLint& location, GLuint slot) noexcept;

		GLint getTextureSlot(const GLint& location) noexcept;


		//跟Texture Array相关 uniform sampler2D texs[10];
		void setTextureArraySlot(const GLint& location, std::vector<GLint> slot) noexcept;

		std::vector<GLint> getTextureArraySlot(const GLint& location) noexcept;

		//返回n个可以使用的textureUnits
		std::vector<GLint> allocateTextureUnits(const int& n);

	private:
		//key:某一个uniform sampler2D tex;变量的location
		//value：GL_TEXTUREXXXX
		std::unordered_map<GLint, GLuint> mTextureSlots{};

		//uniform sampler2D texs[10];
		//name: texs[0] 
		//size:10
		//需要给texs分配十个slot（units）
		//每个purearrayuniform只有一个location
		//key：PureArrayUniform这种类型的texture数组的location 
		//value：数组，为这个texture数组当中的所有textures按序分配的textureUnits
		std::unordered_map<GLint, std::vector<GLint>> mTextureArraySlots{};

		//用于记录当前已经分配到了哪一个TextureUnit
		GLint	mCurrentTextureSlots{ 0 };
	};

	template<>
	void SingleUniform::upload<float>(const float& value) {
		glUniform1f(mLocation, value);
	}

	template<>
	void SingleUniform::upload<glm::vec2>(const glm::vec2& value) {
		//如果要从glm：：vec2这个类型的变量，拿出来其数据指针，就得使用glm::value_ptr
		//这是为了展示多种多样的做法
	//	glUniform2f(mLocation, value.x, value.y);
		glUniform2fv(mLocation, 1, glm::value_ptr(value));
	}

	template<>
	void SingleUniform::upload<glm::vec3>(const glm::vec3& value) {
		glUniform3fv(mLocation, 1, glm::value_ptr(value));
	}

	template<>
	void SingleUniform::upload<glm::vec4>(const glm::vec4& value) {
		glUniform4fv(mLocation, 1, glm::value_ptr(value));
	}

	template<>
	void SingleUniform::upload<int>(const int& value) {
		glUniform1i(mLocation, value);
	}

	template<>
	void SingleUniform::upload<glm::ivec2>(const glm::ivec2& value) {
		glUniform2i(mLocation, value.x, value.y);
	}

	template<>
	void SingleUniform::upload<glm::ivec3>(const glm::ivec3& value) {
		glUniform3i(mLocation, value.x, value.y, value.z);
	}

	template<>
	void SingleUniform::upload<glm::ivec4>(const glm::ivec4& value) {
		glUniform4i(mLocation, value.x, value.y, value.z, value.w);
	}

	template<>
	void SingleUniform::upload<bool>(const bool& value) {
		int v = value;
		glUniform1i(mLocation, v);
	}

	template<>
	void SingleUniform::upload<glm::bvec2>(const glm::bvec2& value) {
		glm::ivec2 v = value;
		glUniform2i(mLocation, v.x, v.y);
	}

	template<>
	void SingleUniform::upload<glm::bvec3>(const glm::bvec3& value) {
		glm::ivec3 v = value;
		glUniform3i(mLocation, v.x, v.y, v.z);
	}

	template<>
	void SingleUniform::upload<glm::bvec4>(const glm::bvec4& value) {
		glm::ivec4 v = value;
		glUniform4i(mLocation, v.x, v.y, v.z, v.w);
	}

	template<>
	void SingleUniform::upload<glm::mat2>(const glm::mat2& value) {
		glUniformMatrix2fv(mLocation, 1, GL_FALSE, glm::value_ptr(value));
	}

	template<>
	void SingleUniform::upload<glm::mat3>(const glm::mat3& value) {
		glUniformMatrix3fv(mLocation, 1, GL_FALSE, glm::value_ptr(value));
	}

	template<>
	void SingleUniform::upload<glm::mat4>(const glm::mat4& value) {
		glUniformMatrix4fv(mLocation, 1, GL_FALSE, glm::value_ptr(value));
	}

	template<>
	void PureArrayUniform::upload<float>(const float* value) {
		glUniform1fv(mLocation, mSize, value);
	}

	template<>
	void PureArrayUniform::upload<glm::vec2>(const glm::vec2* value) {
		//需要拿到数组开头的指针，拿到数组的第一个元素 value【0】，然后取其数据地址
		//数组一串儿数据的开头，与第一个元素的开头相同
		glUniform2fv(mLocation, mSize, glm::value_ptr(value[0]));
	}

	template<>
	void PureArrayUniform::upload<glm::vec3>(const glm::vec3* value) {
		glUniform3fv(mLocation, mSize, glm::value_ptr(value[0]));
	}

	template<>
	void PureArrayUniform::upload<glm::vec4>(const glm::vec4* value) {
		glUniform4fv(mLocation, mSize, glm::value_ptr(value[0]));
	}

	template<>
	void PureArrayUniform::upload<int>(const int* value) {
		glUniform1iv(mLocation, mSize, value);
	}

	template<>
	void PureArrayUniform::upload<glm::ivec2>(const glm::ivec2* value) {
		glUniform2iv(mLocation, mSize, glm::value_ptr(value[0]));
	}

	template<>
	void PureArrayUniform::upload<glm::ivec3>(const glm::ivec3* value) {
		glUniform3iv(mLocation, mSize, glm::value_ptr(value[0]));
	}

	template<>
	void PureArrayUniform::upload<glm::ivec4>(const glm::ivec4* value) {
		glUniform4iv(mLocation, mSize, glm::value_ptr(value[0]));
	}

	template<>
	void PureArrayUniform::upload<glm::mat2>(const glm::mat2* value) {
		glUniformMatrix2fv(mLocation, mSize, GL_FALSE, glm::value_ptr(value[0]));
	}

	template<>
	void PureArrayUniform::upload<glm::mat3>(const glm::mat3* value) {
		glUniformMatrix3fv(mLocation, mSize, GL_FALSE, glm::value_ptr(value[0]));
	}

	template<>
	void PureArrayUniform::upload<glm::mat4>(const glm::mat4* value) {
		glUniformMatrix4fv(mLocation, mSize, GL_FALSE, glm::value_ptr(value[0]));
	}

}