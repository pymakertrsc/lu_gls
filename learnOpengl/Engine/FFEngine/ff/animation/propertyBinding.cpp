#include "propertyBinding.h"
#include "../objects/skinnedMesh.h"

namespace ff {

	PropertyBinding::PropertyBinding(
		const Object3D::Ptr& root,
		const std::string& trackName,
		const size_t& valueSize
	) noexcept {
		//对于每一个KeyFrameTrack，其名字的构成方式为：骨骼名+"."+属性名
		// 所以可以构造一个正则表达式，通过分组匹配，第一个分组装的是骨骼名，第二个分组装的是属性名
		// 举例：
		// trackName = leftArm.position
		//
		std::smatch result;
		//[\\S]匹配任何非空格字符
		std::regex pattern = std::regex("([\\S]+)\\.(\\w+)");

		if (std::regex_search(trackName, result, pattern)) {
			mParseNames.mNodeName = result[1];//leftArm
			mParseNames.mPropertyName = result[2];//position
		}

		mNode = findeNode(root, mParseNames.mNodeName);

		//make buffer
		//todo: we need to know the datatype
		mBuffer = new unsigned char[valueSize * sizeof(float)];
	}

	PropertyBinding::~PropertyBinding() noexcept {
		delete[] mBuffer;
	}

	Object3D::Ptr PropertyBinding::findeNode(const Object3D::Ptr& object, const std::string& nodeName) noexcept {
		if (object->mName == nodeName && object->mIsBone) {
			return object;
		}

		auto children = object->getChildren();
		for (uint32_t i = 0; i < children.size(); ++i) {
			auto child = children[i];
			auto bone = findeNode(child, nodeName);

			if (bone) {
				return bone;
			}
		}

		return nullptr;
	}

	//外界调用AnimationAction进行更新的时候，就会调用本函数
	//apply一旦做完，其保存的mNode（Bone）的某一个属性（position rotation scale）就会更新成插值器计算出来的当前最新值
	void PropertyBinding::apply() noexcept {
		if (mParseNames.mPropertyName == "position") {
			float* buffer = static_cast<float*>(mBuffer);
			mNode->setPosition(buffer[0], buffer[1], buffer[2]);

			mNode->mNeedsUpdateMatrix = true;

			return;
		}

		if (mParseNames.mPropertyName == "quaternion") {
			float* buffer = static_cast<float*>(mBuffer);
			mNode->setQuaternion(buffer[0], buffer[1], buffer[2], buffer[3]);

			mNode->mNeedsUpdateMatrix = true;

			return;
		}

		if (mParseNames.mPropertyName == "scale") {
			float* buffer = static_cast<float*>(mBuffer);
			mNode->setScale(buffer[0], buffer[1], buffer[2]);

			mNode->mNeedsUpdateMatrix = true;

			return;
		}
	}
}