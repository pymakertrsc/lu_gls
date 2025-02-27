#pragma once
#include "../../global/base.h"
#include "../../global/constant.h"
#include "driverTextures.h"
#include "../shaders/uniformsLib.h"

namespace ff {

	class DriverUniforms;

	//һ��uniform���͵ĸ���
	class UniformBase {
	public:
		using Ptr = std::shared_ptr<UniformBase>;

		UniformBase() noexcept;

		~UniformBase() noexcept;

		//uniform������
		std::string mID;

		//��ɸ���uniform����¶�
		virtual void setValue(
			const std::any& value,
			const DriverTextures::Ptr& textures,
			const std::shared_ptr<DriverUniforms>& driverUniforms) {}
	};

	//1 �洢location type����
	//2 �ṩsetValue�Ľӿڣ����������ڲ������ݲ�ͬ���͵�uniform���ò�ͬ��upload

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

		//��T���͵ı���value�����䵽location�������uniform����
		template<typename T>
		void upload(const T& value) {}

		//ģ��ƫ�ػ�
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

	//������������͵�Uniform
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


		//��Texture Array��� uniform sampler2D texs[10];
		void setTextureArraySlot(const GLint& location, std::vector<GLint> slot) noexcept;

		std::vector<GLint> getTextureArraySlot(const GLint& location) noexcept;

		//����n������ʹ�õ�textureUnits
		std::vector<GLint> allocateTextureUnits(const int& n);

	private:
		//key:ĳһ��uniform sampler2D tex;������location
		//value��GL_TEXTUREXXXX
		std::unordered_map<GLint, GLuint> mTextureSlots{};

		//uniform sampler2D texs[10];
		//name: texs[0] 
		//size:10
		//��Ҫ��texs����ʮ��slot��units��
		//ÿ��purearrayuniformֻ��һ��location
		//key��PureArrayUniform�������͵�texture�����location 
		//value�����飬Ϊ���texture���鵱�е�����textures��������textureUnits
		std::unordered_map<GLint, std::vector<GLint>> mTextureArraySlots{};

		//���ڼ�¼��ǰ�Ѿ����䵽����һ��TextureUnit
		GLint	mCurrentTextureSlots{ 0 };
	};

	template<>
	void SingleUniform::upload<float>(const float& value) {
		glUniform1f(mLocation, value);
	}

	template<>
	void SingleUniform::upload<glm::vec2>(const glm::vec2& value) {
		//���Ҫ��glm����vec2������͵ı������ó���������ָ�룬�͵�ʹ��glm::value_ptr
		//����Ϊ��չʾ���ֶ���������
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
		//��Ҫ�õ����鿪ͷ��ָ�룬�õ�����ĵ�һ��Ԫ�� value��0����Ȼ��ȡ�����ݵ�ַ
		//����һ�������ݵĿ�ͷ�����һ��Ԫ�صĿ�ͷ��ͬ
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