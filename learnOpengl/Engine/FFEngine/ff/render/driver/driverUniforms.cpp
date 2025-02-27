#include "driverUniforms.h"
#include "../../log/debugLog.h"
#include "../../wrapper/glWrapper.hpp"

namespace ff {

	UniformBase::UniformBase() noexcept {}

	UniformBase::~UniformBase() noexcept {}

	SingleUniform::SingleUniform(const std::string& id, const GLint& location, const GLenum& type) noexcept :UniformBase() {
		mLocation = location;
		mID = id;
		mType = type;
	}

	SingleUniform::~SingleUniform() noexcept {}

#define UPLOAD(TYPE, VALUE) \
	{\
		TYPE v = std::any_cast<TYPE>(VALUE); \
		upload(v);\
	}

#define UPLOAD_ARRAY(TYPE, VALUE) \
	{\
		auto v = std::any_cast<std::vector<TYPE>>(VALUE); \
		upload(static_cast<TYPE*>(v.data()));\
	}

	void SingleUniform::setValue(
		const std::any& value,
		const DriverTextures::Ptr& textures,
		const std::shared_ptr<DriverUniforms>& driverUniforms)
	{
		// ���ݲ�ͬ��uniform�������ͣ�����ͬ�Ĵ���
		switch (mType) {
		case GL_FLOAT:
			UPLOAD(float, value)
				break;
		case GL_FLOAT_VEC2:
			UPLOAD(glm::vec2, value)
				break;
		case GL_FLOAT_VEC3:
			UPLOAD(glm::vec3, value)
				break;
		case GL_FLOAT_VEC4:
			UPLOAD(glm::vec4, value)
				break;
		case GL_INT:
			UPLOAD(int, value)
				break;
		case GL_INT_VEC2:
			UPLOAD(glm::ivec2, value)
				break;
		case GL_INT_VEC3:
			UPLOAD(glm::ivec3, value)
				break;
		case GL_INT_VEC4:
			UPLOAD(glm::ivec4, value)
				break;
		case GL_BOOL:
			UPLOAD(bool, value)
				break;
		case GL_BOOL_VEC2:
			UPLOAD(glm::bvec2, value)
				break;
		case GL_BOOL_VEC3:
			UPLOAD(glm::bvec3, value)
				break;
		case GL_BOOL_VEC4:
			UPLOAD(glm::bvec4, value)
				break;
		case GL_FLOAT_MAT2:
			UPLOAD(glm::mat2, value)
				break;
		case GL_FLOAT_MAT3:
			UPLOAD(glm::mat3, value)
				break;
		case GL_FLOAT_MAT4:
			UPLOAD(glm::mat4, value)
				break;
		case GL_SAMPLER_2D:
			uploadTexture(driverUniforms, textures, value);
			break;
		case GL_SAMPLER_CUBE:
			uploadTexture(driverUniforms, textures, value);
			break;
		default:
			break;
		}
	}

	void SingleUniform::uploadTexture(
		const std::shared_ptr<DriverUniforms>& driverUniforms,
		const DriverTextures::Ptr& textures,
		const std::any& value)
	{

		Texture::Ptr texture = nullptr;
		if (value.type() == typeid(Texture::Ptr)) {
			texture = std::any_cast<Texture::Ptr>(value);
		}
		else if (value.type() == typeid(CubeTexture::Ptr)) {
			texture = std::any_cast<CubeTexture::Ptr>(value);
		}

		//��ǰ��SingleUniformһ����һ��sampler2D��unifrom
		//��singleUniform��ʼ��������ʱ�򣬾��Ѿ��õ����Լ���location
		//��DriverUniforms���棬��ͼ��ȡ��uniform����Ӧ��textureUnit
		auto textureSlot = driverUniforms->getTextureSlot(mLocation);
		
		//�����û�и���ǰ��texture Uniform����textureUnit����ô�͵����·���һ��
		if (textureSlot < 0) {
			textureSlot = driverUniforms->allocateTextureUnits(1)[0];

			//����֮����Ҫ��driverUniforms�����¼����
			driverUniforms->setTextureSlot(mLocation, textureSlot);
		}

		//���»�õĻ����Ѿ�������ϵ�textureUnit���뵱ǰtexture��Ӧ��DriverTexture�е�mHanlde���а�
		textures->bindTexture(texture, textureSlot);

		//��shader���е�sampler��textureUnit���а�
		//�����sampler����һ�����������¶�Ӧ
		// GL_TEXTURE0 �ʹ���0
		// GL_TEXTURE1 �ʹ���1
		// GL_TEXTURE2 �ʹ���2
		// 
		// textureSlot������GL_TEXTURE2����16��������
		// ֻ��Ҫ��textureSlot - GL_TEXTURE0�Ϳ��Եõ��������
		//
		GLint textureIndex = textureSlot - GL_TEXTURE0;
		gl::uniform1i(mLocation, textureIndex);
	}

	PureArrayUniform::PureArrayUniform(const std::string& id, const GLint& location, const GLenum& type, GLint size) noexcept :UniformBase() {
		mLocation = location;
		mID = id;
		mType = type;
		mSize = size;
	}

	PureArrayUniform::~PureArrayUniform() noexcept {}

	void PureArrayUniform::setValue(
		const std::any& value,
		const DriverTextures::Ptr& textures,
		const std::shared_ptr<DriverUniforms>& driverUniforms)
	{
		switch (mType) {
		case GL_FLOAT:
			UPLOAD_ARRAY(float, value)
				break;
		case GL_FLOAT_VEC2:
			UPLOAD_ARRAY(glm::vec2, value)
				break;
		case GL_FLOAT_VEC3:
			UPLOAD_ARRAY(glm::vec3, value)
				break;
		case GL_FLOAT_VEC4:
			UPLOAD_ARRAY(glm::vec4, value)
				break;
		case GL_INT:
			UPLOAD_ARRAY(int, value)
				break;
		case GL_INT_VEC2:
			UPLOAD_ARRAY(glm::ivec2, value)
				break;
		case GL_INT_VEC3:
			UPLOAD_ARRAY(glm::ivec3, value)
				break;
		case GL_INT_VEC4:
			UPLOAD_ARRAY(glm::ivec4, value)
				break;
		case GL_BOOL:
			UPLOAD_ARRAY(int, value)
				break;
		case GL_BOOL_VEC2:
			UPLOAD_ARRAY(glm::ivec2, value)
				break;
		case GL_BOOL_VEC3:
			UPLOAD_ARRAY(glm::ivec3, value)
				break;
		case GL_BOOL_VEC4:
			UPLOAD_ARRAY(glm::ivec4, value)
				break;
		case GL_FLOAT_MAT2:
			UPLOAD_ARRAY(glm::mat2, value)
				break;
		case GL_FLOAT_MAT3:
			UPLOAD_ARRAY(glm::mat3, value)
				break;
		case GL_FLOAT_MAT4:
			UPLOAD_ARRAY(glm::mat4, value)
				break;
		case GL_SAMPLER_2D:
			uploadTexture2DArray(driverUniforms, textures, value);
			break;
		default:
			break;
		}
	}

	void PureArrayUniform::uploadTexture2DArray(
		const std::shared_ptr<DriverUniforms>& driverUniforms,
		const DriverTextures::Ptr& textures,
		const std::any& value)
	{

		//PureArrayUniform ��Ӧ���ⲿ���ݣ�һ������װ����vector����
		auto textureArray = std::any_cast<std::vector<Texture::Ptr>>(value);

		//���豾���鳤��Ϊn����ô����ҪΪn��textures����textureUnits
		std::vector<GLint> textureSlots = driverUniforms->getTextureArraySlot(mLocation);

		//������鳤��Ϊ0��˵��ԭ�Ȳ�û�з����
		if (textureSlots.size() == 0) {
			//������PureArrayUniform������һ����������ʽ��
			// uniform sampler2D texs[10];
			// mSize����10
			//
			textureSlots = driverUniforms->allocateTextureUnits(mSize);
			driverUniforms->setTextureArraySlot(mLocation, textureSlots);
		}

		//����õ�n��textureUnits���ֱ���texture���鵱�е�ÿһ��texture���а�
		// ������
		// textureArray[0]-GL_TEXTURE4
		// textureArray[1]-GL_TEXTURE5
		// textureArray[2]-GL_TEXTURE6

		for (uint32_t i = 0; i < textureArray.size(); ++i) {
			textures->bindTexture(textureArray[i], textureSlots[i]);
		}


		//��shader���е�sampler2D���飬��textureUnits֮��Ĺ�ϵ

		std::vector<GLint> textureIndices;
		for (int i = 0; i < textureSlots.size(); ++i) {
			textureIndices.push_back(textureSlots[i] - GL_TEXTURE0);
		}

		//���磺
		// uniform sampler2D texs[3];
		// texs[0]-4
		// texs[1]-5
		// texs[2]-6
		//
		gl::uniform1iv(mLocation, textureArray.size(), textureIndices.data());
	}

	StructuredUniform::StructuredUniform(const std::string& id) noexcept : UniformBase(), UniformContainer() {
		mID = id;
	}

	StructuredUniform::~StructuredUniform() noexcept {}

	void StructuredUniform::setValue(
		const std::any& value,
		const DriverTextures::Ptr& textures,
		const std::shared_ptr<DriverUniforms>& driverUniforms)
	{
		//��������value��Ȼ�Ǹ�any�������������setvalue��StructuredUniform��setValue����������
		//���anyһ����һ��UniformUnitMap-map<string, any>
		auto v = std::any_cast<UniformUnitMap>(value);

		//���������Ѿ�����������uniformMap
		for (const auto& iter : mUniformMap) {
			auto name = iter.first;
			auto uniform = iter.second;

			//single pureArray structured ����uniform������ʵ���˸��Ե�setValue
			//�����single��mValue�����Ǹ�matrixҲ�����Ǹ�float�ȵ�
			//�����pureArray��mValue������һ��vector<T>
			//�����structured,mValue������һ��UniformUnitMap
			uniform->setValue(v[name], textures, driverUniforms);
		}
	}

	//�յ����������program���������е�activeUniforms���ó������Ϣ
	//ÿ�ó���һ����Ϣ����Ҫ����������ʽƥ�䣬�Ӷ���Ӧ��������uniforms���е�һ��
	//��������̵��У��γ���Uniforms�Ĳ㼶�ܹ�
	DriverUniforms::DriverUniforms(const GLint& program) noexcept : UniformContainer() {
		//��õ�ǰprogram���Ѿ������uniforms������
		GLint count = 0;
		glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &count);

		UniformContainer* container = this;

		// ����ÿһ��Uniform�Ĳ�������
		GLint location = 0;//��������uniform��location
		GLsizei bufferSize = 256;//����opengl�ӿڵ�namebuffer�Ĵ�С
		GLsizei length;//��uniform��������ʵ����
		GLint size;//�����uniform��һ��PureArray����ô�ͷ������鳤��
		GLenum type;//��uniform���߱�Uniform����Ԫ�ص�����,structuredUniform��չ�������ͣ�����t.f;
		GLchar name[256];//��uniform������

		for (uint32_t i = 0; i < count; ++i) {
			glGetActiveUniform(program, i, bufferSize, &length, &size, &type, name);
			location = glGetUniformLocation(program, name);

			//������ʽ����
			// (\\w+) ƥ��1-����ַ�(��ĸ�����»��ߣ�
			// (\\])?  []��������ʽ���У����ع��ܣ�����[a-z]����ʾƥ��һ��],?�����ǰ���ı��ʽ����ƥ��Ҳ����ƥ�䲻��
			// (\\[|\\.)? ƥ��һ��[����ƥ��һ��.��ƥ�䲻��Ҳû��ϵ
			//
			std::string pattern = "(\\w+)(\\])?(\\[|\\.)?";
			std::regex reg(pattern);

			//result[0] ƥ������������ִ�
			// result[1] ƥ��(\\w+)������ִ�
			//result[2] ƥ��(\\])������ִ�
			//result[3] ƥ��(\\[|\\.)������ִ�
			std::smatch result;

			std::string text = name;
			std::string id;
			std::string subscript;
			bool		idIsIndex{ false };
			size_t		matchEnd{ 0 };

			//don not forget!!
			container = this;

			//if it is an pureArray, then openGL gets xxx[0],so matchEnd is xxx[

			//1 ��ÿ��Uniform�����ֽ���ѭ��ƥ�䡣
			//2 ÿһ��ƥ��õ�result֮��,��������жϣ�������ǰUniform������
			//3 �����StructuredUniform����ô�ͽ��в㼶�ܹ��Ľ���
			//
			while (true) {
				if (std::regex_search(text, result, reg)) {
					id = result[1].str();
					subscript = result[3].str();
					matchEnd = result.position(0) + result.length(0);

					if (subscript.empty() || (subscript == "[" && matchEnd + 2 == text.length())) {
						UniformBase::Ptr uniformObject = nullptr;

						//����SingleUniform����PureArrayUniform
						if (subscript.empty()) {
							uniformObject = SingleUniform::create(id, location, type);
						}
						else {
							uniformObject = PureArrayUniform::create(id, location, type, size);
						}

						//�����ɵ�UniformObject���뵽��ǰ��Container
						addUniform(container, uniformObject);

						//����Ҫ����
						break;
					}
					else {
						StructuredUniform::Ptr next = nullptr;

						//�ڵ�ǰ��Container���棬�Ƿ��Ѿ�����������Ϊ��id��StructuredUniform
						//����У���������Uniforms�ǣ�����װ����StructuredUniform֮��
						auto uniformMap = container->mUniformMap;
						auto iter = uniformMap.find(id);

						if (iter == uniformMap.end()) {
							next = StructuredUniform::create(id);
							addUniform(container, next);
						}
						else {
							next = std::dynamic_pointer_cast<StructuredUniform>(iter->second);
						}

						container = next.get();
					}

					text = result.suffix().str();
				}
				else {
					break;
				}
			}

			DebugLog::getInstance()->printUniformInfo(i, length, size, type, name);
		}
	}

	DriverUniforms::~DriverUniforms() noexcept {}

	void DriverUniforms::upload(UniformHandleMap& uniformHandleMap, const DriverTextures::Ptr& textures) {
		//�����ı�׼��DriverUniforms�ڲ��Ѿ������õ�Uniforms�ܹ���
		for (auto& iter : mUniformMap) {
			auto name = iter.first;
			auto uniform = iter.second;//UniformBase����������������֮һ

			//ȡ����UniformHandle
			auto& uniformHandle = uniformHandleMap[name];

			if (uniformHandle.mNeedsUpdate) {
				uniformHandle.mNeedsUpdate = false;

				//single pureArray structured ����uniform������ʵ���˸��Ե�setValue
				//�����single��mValue�����Ǹ�matrixҲ�����Ǹ�float�ȵ�
				//�����pureArray��mValue������һ��vector<T>
				//�����structured,mValue������һ��UniformUnitMap
				uniform->setValue(uniformHandle.mValue, textures, shared_from_this());
			}
		}
	}

	void DriverUniforms::addUniform(UniformContainer* container, const UniformBase::Ptr& uniformObject) {
		container->mUniformMap.insert(std::make_pair(uniformObject->mID, uniformObject));
	}

	void DriverUniforms::setTextureSlot(const GLint& location, GLuint slot) noexcept {
		mTextureSlots.insert(std::make_pair(location, slot));
	}

	GLint DriverUniforms::getTextureSlot(const GLint& location) noexcept {
		auto iter = mTextureSlots.find(location);
		if (iter != mTextureSlots.end()) {
			return iter->second;
		}

		return -1;
	}

	void DriverUniforms::setTextureArraySlot(const GLint& location, std::vector<GLint> slots) noexcept {
		mTextureArraySlots.insert(std::make_pair(location, slots));
	}

	std::vector<GLint> DriverUniforms::getTextureArraySlot(const GLint& location) noexcept {
		std::vector<GLint> slots;
		auto iter = mTextureArraySlots.find(location);
		if (iter != mTextureArraySlots.end()) {
			slots = iter->second;
		}

		return slots;
	}

	std::vector<GLint> DriverUniforms::allocateTextureUnits(const int& n) {
		std::vector<GLint> units;
		for (int i = 0; i < n; ++i) {
			//GL_TEXTURE1 = GL_TEXTURE0 + 1
			//GL_TEXTURE2 = GL_TEXTURE0 + 2
			//...........
			GLenum slot = GL_TEXTURE0 + mCurrentTextureSlots;
			if (slot >= MAX_TEXTURE) {
				throw std::runtime_error("DriverTextures->allocateTextureUnit: too much textures");
			}

			units.push_back(slot);
			mCurrentTextureSlots++;
		}

		return units;
	}
}