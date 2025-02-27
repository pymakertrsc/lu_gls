#include "propertyBinding.h"
#include "../objects/skinnedMesh.h"

namespace ff {

	PropertyBinding::PropertyBinding(
		const Object3D::Ptr& root,
		const std::string& trackName,
		const size_t& valueSize
	) noexcept {
		//����ÿһ��KeyFrameTrack�������ֵĹ��ɷ�ʽΪ��������+"."+������
		// ���Կ��Թ���һ��������ʽ��ͨ������ƥ�䣬��һ������װ���ǹ��������ڶ�������װ����������
		// ������
		// trackName = leftArm.position
		//
		std::smatch result;
		//[\\S]ƥ���κηǿո��ַ�
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

	//������AnimationAction���и��µ�ʱ�򣬾ͻ���ñ�����
	//applyһ�����꣬�䱣���mNode��Bone����ĳһ�����ԣ�position rotation scale���ͻ���³ɲ�ֵ����������ĵ�ǰ����ֵ
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