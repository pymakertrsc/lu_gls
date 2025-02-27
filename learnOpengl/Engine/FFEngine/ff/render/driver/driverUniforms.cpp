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
		// 根据不同的uniform变量类型，做不同的处理
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

		//当前的SingleUniform一定是一个sampler2D的unifrom
		//在singleUniform初始化创建的时候，就已经拿到了自己的location
		//从DriverUniforms里面，试图获取本uniform所对应的textureUnit
		auto textureSlot = driverUniforms->getTextureSlot(mLocation);
		
		//如果还没有给当前的texture Uniform分配textureUnit，那么就得重新分配一个
		if (textureSlot < 0) {
			textureSlot = driverUniforms->allocateTextureUnits(1)[0];

			//分配之后，需要在driverUniforms里面记录下来
			driverUniforms->setTextureSlot(mLocation, textureSlot);
		}

		//将新获得的或者已经分配完毕的textureUnit，与当前texture对应的DriverTexture中的mHanlde进行绑定
		textures->bindTexture(texture, textureSlot);

		//将shader当中的sampler与textureUnit进行绑定
		//传输给sampler的是一个整数，如下对应
		// GL_TEXTURE0 就传输0
		// GL_TEXTURE1 就传输1
		// GL_TEXTURE2 就传输2
		// 
		// textureSlot是诸如GL_TEXTURE2这种16进制数字
		// 只需要用textureSlot - GL_TEXTURE0就可以得到编号数字
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

		//PureArrayUniform 对应的外部数据，一定都被装载了vector里面
		auto textureArray = std::any_cast<std::vector<Texture::Ptr>>(value);

		//假设本数组长度为n，那么就需要为n个textures分配textureUnits
		std::vector<GLint> textureSlots = driverUniforms->getTextureArraySlot(mLocation);

		//如果数组长度为0，说明原先并没有分配过
		if (textureSlots.size() == 0) {
			//由于是PureArrayUniform，所以一定是如下形式：
			// uniform sampler2D texs[10];
			// mSize就是10
			//
			textureSlots = driverUniforms->allocateTextureUnits(mSize);
			driverUniforms->setTextureArraySlot(mLocation, textureSlots);
		}

		//将获得的n个textureUnits，分别与texture数组当中的每一个texture进行绑定
		// 举例：
		// textureArray[0]-GL_TEXTURE4
		// textureArray[1]-GL_TEXTURE5
		// textureArray[2]-GL_TEXTURE6

		for (uint32_t i = 0; i < textureArray.size(); ++i) {
			textures->bindTexture(textureArray[i], textureSlots[i]);
		}


		//绑定shader当中的sampler2D数组，与textureUnits之间的关系

		std::vector<GLint> textureIndices;
		for (int i = 0; i < textureSlots.size(); ++i) {
			textureIndices.push_back(textureSlots[i] - GL_TEXTURE0);
		}

		//比如：
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
		//传进来的value虽然是个any，但是我们这个setvalue是StructuredUniform的setValue函数，所以
		//这个any一定是一个UniformUnitMap-map<string, any>
		auto v = std::any_cast<UniformUnitMap>(value);

		//遍历的是已经解析出来的uniformMap
		for (const auto& iter : mUniformMap) {
			auto name = iter.first;
			auto uniform = iter.second;

			//single pureArray structured 三种uniform都各自实现了各自的setValue
			//如果是single，mValue可能是个matrix也可能是个float等等
			//如果是pureArray，mValue必须是一个vector<T>
			//如果是structured,mValue必须是一个UniformUnitMap
			uniform->setValue(v[name], textures, driverUniforms);
		}
	}

	//收到了外界给与的program，解析其中的activeUniforms，拿出相关信息
	//每拿出来一个信息，都要进行正则表达式匹配，从而对应生成三种uniforms当中的一种
	//在这个过程当中，形成了Uniforms的层级架构
	DriverUniforms::DriverUniforms(const GLint& program) noexcept : UniformContainer() {
		//获得当前program中已经激活的uniforms的数量
		GLint count = 0;
		glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &count);

		UniformContainer* container = this;

		// 关于每一个Uniform的参数声明
		GLint location = 0;//用来传输uniform的location
		GLsizei bufferSize = 256;//给到opengl接口的namebuffer的大小
		GLsizei length;//本uniform的名字真实长度
		GLint size;//如果本uniform是一个PureArray，那么就返回数组长度
		GLenum type;//本uniform或者本Uniform数组元素的类型,structuredUniform是展开后类型，比如t.f;
		GLchar name[256];//本uniform的名字

		for (uint32_t i = 0; i < count; ++i) {
			glGetActiveUniform(program, i, bufferSize, &length, &size, &type, name);
			location = glGetUniformLocation(program, name);

			//正则表达式解析
			// (\\w+) 匹配1-多个字符(字母数字下划线）
			// (\\])?  []在正则表达式当中，独特功能，比如[a-z]。表示匹配一个],?表达了前方的表达式可以匹配也可以匹配不到
			// (\\[|\\.)? 匹配一个[或者匹配一个.，匹配不到也没关系
			//
			std::string pattern = "(\\w+)(\\])?(\\[|\\.)?";
			std::regex reg(pattern);

			//result[0] 匹配上述规则的字串
			// result[1] 匹配(\\w+)规则的字串
			//result[2] 匹配(\\])规则的字串
			//result[3] 匹配(\\[|\\.)规则的字串
			std::smatch result;

			std::string text = name;
			std::string id;
			std::string subscript;
			bool		idIsIndex{ false };
			size_t		matchEnd{ 0 };

			//don not forget!!
			container = this;

			//if it is an pureArray, then openGL gets xxx[0],so matchEnd is xxx[

			//1 对每个Uniform的名字进行循环匹配。
			//2 每一次匹配得到result之后,都会进行判断，决定当前Uniform的类型
			//3 如果是StructuredUniform，那么就进行层级架构的建设
			//
			while (true) {
				if (std::regex_search(text, result, reg)) {
					id = result[1].str();
					subscript = result[3].str();
					matchEnd = result.position(0) + result.length(0);

					if (subscript.empty() || (subscript == "[" && matchEnd + 2 == text.length())) {
						UniformBase::Ptr uniformObject = nullptr;

						//生成SingleUniform或者PureArrayUniform
						if (subscript.empty()) {
							uniformObject = SingleUniform::create(id, location, type);
						}
						else {
							uniformObject = PureArrayUniform::create(id, location, type, size);
						}

						//将生成的UniformObject加入到当前的Container
						addUniform(container, uniformObject);

						//很重要！！
						break;
					}
					else {
						StructuredUniform::Ptr next = nullptr;

						//在当前的Container里面，是否已经含有了名字为本id的StructuredUniform
						//如果有，接下来的Uniforms们，都会装到本StructuredUniform之下
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
		//遍历的标准是DriverUniforms内部已经解析好的Uniforms架构们
		for (auto& iter : mUniformMap) {
			auto name = iter.first;
			auto uniform = iter.second;//UniformBase，可能是三种类型之一

			//取出来UniformHandle
			auto& uniformHandle = uniformHandleMap[name];

			if (uniformHandle.mNeedsUpdate) {
				uniformHandle.mNeedsUpdate = false;

				//single pureArray structured 三种uniform都各自实现了各自的setValue
				//如果是single，mValue可能是个matrix也可能是个float等等
				//如果是pureArray，mValue必须是一个vector<T>
				//如果是structured,mValue必须是一个UniformUnitMap
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