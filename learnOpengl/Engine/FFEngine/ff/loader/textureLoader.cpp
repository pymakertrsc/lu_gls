#include "textureLoader.h"
#include <stb_image.h>
#include "../global/config.h"
#include "cache.h"

namespace ff {

	TextureLoader::TextureLoader() noexcept {}

	TextureLoader::~TextureLoader() noexcept {}

	Texture::Ptr TextureLoader::load(const std::string& path, unsigned char* dataIn, uint32_t widthIn, uint32_t heightIn) {
		Texture::Ptr texture = nullptr;
		std::string filePath = path;

		//如果路径为空，则使用默认图片
		if (filePath.empty()) {
			filePath = DefaultTexturePath;
		}

		//检查是否已经生成过source，如果生成了就从cache里面取出来
		Source::Ptr source = Cache::getInstance()->getSource(path);

		if (source) {
			texture = Texture::create(source->mWidth, source->mHeight);
			texture->mSource = source;
		}
		else {
			source = Source::create();
			//以下数据都是新数据,所以false掉
			source->mNeedsUpdate = false;

			//使用引用类型，可以直接对data进行更改，结果会同步到source的Data当中
			auto& data = source->mData;

			int			picType = 0;
			int width = 0, height = 0;

			//整个读取出来的图片数据大小
			uint32_t dataSize{ 0 };

			//读取出来的图片数据指针
			unsigned char* bits{ nullptr };

			//要么从硬盘读取，要么从数据流读取
			if (dataIn == nullptr) {
				//if nofile, use default
				std::fstream file(filePath);
				if (!file.is_open()) {
					filePath = DefaultTexturePath;
				}
				else {
					file.close();
				}

				bits = stbi_load(filePath.c_str(), &width, &height, &picType, toStbImageFormat(TextureFormat::RGBA));
			}
			else {
				//记录了整个数据的大小
				uint32_t dataInSize = 0;

				//一个fbx模型有可能打包进来jpg，带有压缩格式的图片情况下，height可能为0，width就代表了整个图片的大小
				if (!heightIn) {
					dataInSize = widthIn;
				}
				else {
					dataInSize = widthIn * heightIn;
				}

				//我们现在拿到的dataIn，并不是展开的位图数据，有可能是一个jpg png等格式的图片数据流
				bits = stbi_load_from_memory(dataIn, dataInSize, &width, &height, &picType, toStbImageFormat(TextureFormat::RGBA));
			}

			dataSize = width * height * toByteSize(TextureFormat::RGBA);

			//经过上述过程，终于准备好了所有的必要数据，接下来填充source的data(Vector<Byte>)
			if (dataSize && bits) {
				data.resize(dataSize);

				//从bits向data的地址开头，拷贝dataSize个Byte的数据
				memcpy(data.data(), bits, dataSize);
			}

			//此时，bits指向的由stbimage给到的内存数据，就delete了
			stbi_image_free(bits);

			source->mWidth = width;
			source->mHeight = height;
			texture = Texture::create(source->mWidth, source->mHeight);
		}


		texture->mSource = source;
		Cache::getInstance()->cacheSource(filePath, source);

		return texture;
	}
}