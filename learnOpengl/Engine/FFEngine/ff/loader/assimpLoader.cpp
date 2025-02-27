#include "assimpLoader.h"
#include "../objects/mesh.h"
#include "../objects/skinnedMesh.h"
#include "../objects/group.h"
#include "../material/meshPhongMaterial.h"
#include "../material/cubeMaterial.h"
#include "../material/depthMaterial.h"
#include "../material/meshBasicMaterial.h"
#include "../loader/textureLoader.h"
#include "../loader/cache.h"
#include "../animation/animationAction.h"
#include "../animation/animationClip.h"
#include "../animation/keyframeTracks/vectorKeyframeTrack.h"
#include "../animation/keyframeTracks/quaternionKeyframeTrack.h"

namespace ff {

	AssimpResult::Ptr AssimpLoader::load(const std::string& path) noexcept {
		AssimpResult::Ptr result = AssimpResult::create();

		//当前模型所有Mesh用到的material都会记录在这样的数组里面，顺序按照aiScene里的
		//mMaterials的顺序相同
		std::vector<Material::Ptr> materials;

		//生成根节点
		Object3D::Ptr rootObject = Group::create();

		//骨骼动画相关
		Bone::Ptr rootBone = Bone::create();
		std::vector<Bone::Ptr> bones{};

		//开始进行读取
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(
			path, 
			aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "Error:model read fail!" << std::endl;
			return nullptr;
		}

		//模型读取的path一般是这样的：assets/models/superMan/man.fbx
		//取出来根路径：assets/models/superMan/
		std::size_t lastIndex = path.find_last_of("//");
		std::string rootPath = path.substr(0, lastIndex + 1);

		//if has animations, then processSkeleton
		if (scene->mNumAnimations) {
			processSkeleton(scene->mRootNode, scene, rootBone, bones);

			rootObject->addChild(rootBone);
		}

		processMaterial(scene, rootPath, materials);
		processNode(scene->mRootNode, scene, rootObject, materials, bones);

		//make actions
		//读取所有动画的关键帧数据，为每个动画构建AnimationAction
		std::vector<AnimationAction::Ptr> actions{};
		if (scene->mNumAnimations) {
			auto clips = processAnimation(scene);
			for (uint32_t c = 0; c < clips.size(); ++c) {
				auto action = AnimationAction::create(clips[c], rootObject);
				actions.push_back(action);
			}
		}

		result->mActions = actions;
		result->mObject = rootObject;
		return result;
	}

	//1 解析每个Node，如果有Mesh，就解析生成Mesh，并且加入到本Node对应的Group对象里面
	//2 如果系统没有动画并且本Node没有Mesh，则将LocalTransform设置到对应的Group的localMatrix上面
	//3 建设层级架构
	//
	void AssimpLoader::processNode(
		const aiNode* node,
		const aiScene* scene,
		Object3D::Ptr parentObject,
		const std::vector<Material::Ptr>& materials,
		const std::vector<Bone::Ptr>& bones) {

		//make a group for all the meshes in the node
		Group::Ptr group = Group::create();
		for (uint32_t i = 0; i < node->mNumMeshes; ++i) {
			//对于当前node的第i个Mesh，取出来其MeshID（node->mMeshes[i]），用这个MeshID向Scene里面索引aiMesh，拿到具体数据
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			group->addChild(processMesh(mesh, scene, getGLMMat4(node->mTransformation), materials, bones));
		}

		parentObject->addChild(group);

		//if node has meshes, then nodeTransform is judged by processMesh
		//因为有动画的时候，本节点的mesh的WorldMatrix变换，应该听命于影响它的骨骼节点
		if (!scene->mNumAnimations && !node->mNumMeshes) {
			//add transform for all meshes in the node
			glm::mat4 localMatrix = getGLMMat4(node->mTransformation);
			group->setLocalMatrix(localMatrix);
		}

		for (uint32_t i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene, group, materials, bones);
		}
	}

	void AssimpLoader::processSkeleton(
		const aiNode* node,
		const aiScene* scene,
		Bone::Ptr parentBone,
		std::vector<Bone::Ptr>& bones) {
		//name
		auto bone = Bone::create();
		bones.push_back(bone);

		bone->mName = node->mName.C_Str();

		//transform
		glm::mat4 nodeMatrix = getGLMMat4(node->mTransformation);
		bone->mNodeMatrix = nodeMatrix;

		parentBone->addChild(bone);

		for (uint32_t i = 0; i < node->mNumChildren; ++i) {
			processSkeleton(node->mChildren[i], scene, bone, bones);
		}
	}

	void AssimpLoader::processMaterial(
		const aiScene* scene,
		const std::string& rootPath,
		std::vector<Material::Ptr>& materials)
	{
		//循环解析aiScene里面的每一个material
		for (uint32_t id = 0; id < scene->mNumMaterials; ++id) {
			Material::Ptr material{ nullptr };

			aiMaterial* aimaterial = scene->mMaterials[id];

			//用来获取当前的Material是什么类型
			aiShadingMode shadingMode{ aiShadingMode::aiShadingMode_Phong };

			aimaterial->Get(AI_MATKEY_SHADING_MODEL, shadingMode);

			//todo: we may need more material model
			//由于我们只实现了MeshPhongmaterial这种模型通用的材质，所以一律写成PhongMaterial
			switch (shadingMode) {
			case aiShadingMode::aiShadingMode_Phong:
				material = MeshPhongMaterial::create();
				break;
			default:
				material = MeshPhongMaterial::create();
				break;
			}

			//开始读取每个Material的贴图数据
			material->mDiffuseMap = processTexture(aiTextureType_DIFFUSE, scene, aimaterial, rootPath);

			material->mNormalMap = processTexture(aiTextureType_NORMALS, scene, aimaterial, rootPath);
					
			material->mSpecularMap = processTexture(aiTextureType_SPECULAR, scene, aimaterial, rootPath);
			
			materials.push_back(material);
		}
	}

	Texture::Ptr AssimpLoader::processTexture(
		const aiTextureType& type,
		const aiScene* scene,
		const aiMaterial* material,
		const std::string& rootPath) {
		//for texture
		aiString aiPath;

		//for now we only need one texture per type, without texture blending
		//todo: multi-texture blending
		//对于当前模型，我们工程里面存储的路径可能是：assets/models/superMan/man.fbx
		//图片数据可能会存放在：assets/models/superMan/textures/文件夹下
		//下方拿到的aiPath是要读取的图片的相对路径，相对于man.fbx
		material->Get(AI_MATKEY_TEXTURE(type, 0), aiPath);

		if (!aiPath.length) {
			return nullptr;
		}

		//有的模型，会把纹理一起打包在模型内部，并没有单独存放。
		//查看对于当前的aiPath对应的图片来讲，是否存在这种打包在模型内部的情况
		const aiTexture* assimpTexture = scene->GetEmbeddedTexture(aiPath.C_Str());
		if (assimpTexture) {
			//如果确实图片打包在了模型内部，则上述代码获取到的aiTexture里面就含有了图片数据
			unsigned char* dataIn = reinterpret_cast<unsigned char*>(assimpTexture->pcData);
			uint32_t widthIn = assimpTexture->mWidth;
			uint32_t heightIn = assimpTexture->mHeight;
			std::string path = aiPath.C_Str();

			return TextureLoader::load(path, dataIn, widthIn, heightIn);
		}
		//因为aiPath是textures/diffuseTexture.jpg
		//拼装后变成：assets/models/superMan/textures/diffuseTexture.jpg
		std::string fullPath = rootPath + aiPath.C_Str();
		return TextureLoader::load(fullPath);
	}

	//1 如果当前解析的Mesh收到了骨骼的影响，那么就是一个SkinnedMesh
	//2 如果收到了骨骼影响，那么其Attribute就需要加入skinIdex以及skinWeight
	Object3D::Ptr AssimpLoader::processMesh(
		const aiMesh* mesh,
		const aiScene* scene,
		//当前Mesh所属的Node如果有哪怕一个mesh，就无法设置LocalMatrix
		const glm::mat4 localTransform,
		const std::vector<Material::Ptr>& materials,
		const std::vector<Bone::Ptr>& bones) {

		Object3D::Ptr object = nullptr;
		Material::Ptr material = nullptr;
		Geometry::Ptr geometry = Geometry::create();

		std::vector<Bone::Ptr> meshBones;//对当前Mesh产生影响的bone的数组
		std::vector<glm::mat4> offsetMatrices;//对应上述数组的每一个bone的offsetMatrix

		//一个Mesh所需要的所有attributes
		std::vector<float> positions;
		std::vector<float> normals;
		std::vector<float> tangents;
		std::vector<float> bitangents;

		std::vector<std::vector<float>> uvs;//每一个元素都是第i个channel的纹理坐标数据
		std::vector<uint32_t> numUVComponents;//每一个元素都是第i个channel的itemSize

		std::vector<uint32_t> indices;
		std::vector<float> skinIndices;
		std::vector<float> skinWeights;

		//按照顶点来遍历的
		for (uint32_t i = 0; i < mesh->mNumVertices; ++i) {
			positions.push_back(mesh->mVertices[i].x);
			positions.push_back(mesh->mVertices[i].y);
			positions.push_back(mesh->mVertices[i].z);

			normals.push_back(mesh->mNormals[i].x);
			normals.push_back(mesh->mNormals[i].y);
			normals.push_back(mesh->mNormals[i].z);

			if (mesh->mTangents->Length()) {
				tangents.push_back(mesh->mTangents[i].x);
				tangents.push_back(mesh->mTangents[i].y);
				tangents.push_back(mesh->mTangents[i].z);
			}

			if (mesh->mBitangents->Length()) {
				bitangents.push_back(mesh->mBitangents[i].x);
				bitangents.push_back(mesh->mBitangents[i].y);
				bitangents.push_back(mesh->mBitangents[i].z);
			}

			//may have multi-textures, u is the number
			//1 一个模型可能其中的mesh会有多个贴图，贴图可能会有相同功能，比如DiffuseMap可能有多张，那么颜色就会混合
			//也有可能是不同功能，比如NormalMap SpecularMap DiffuseMap。
			// 2 既然有多张贴图可能，同一个顶点采样不同的纹理，可能会有不同的uv坐标
			// 3 纹理坐标会有不同类型，如果采样二位图片，就是简单的二位uv，如果采样环境贴图uvw（str）
			// 
			// 总结：对于同一个Mesh，读取其某个顶点的uv
			// 1 会有多套uv，分布在不同的Channel
			// 2 读取纹理坐标的时候，要判断是uv还是uvw
			//

			//GetNumUVChannels:获取当前Mesh有多少套纹理坐标
			//将不同的Channel的纹理坐标存在了不同的vector<float>里面
			for (uint32_t u = 0; u < mesh->GetNumUVChannels(); ++u) {
				if (u >= uvs.size()) {
					uvs.push_back(std::vector<float>());
				}
				std::vector<float>& uvComponents = uvs[u];

				//查看对于当前这个Channel其纹理坐标是uv还是uvw
				//mNumUVComponents 存储了当前第u个Channel所对应的这一套纹理坐标itemSize
				uint32_t numComponents = mesh->mNumUVComponents[u];

				//uv  or  uvw 存下来的原因，是如果将当前的纹理坐标作为attribute传入geometry，在构建
				//attribute的时候，就需要知道itemSize
				if (u >= numUVComponents.size()) {
					numUVComponents.push_back(numComponents);
				}

				//按照numComponents进行遍历读取，要么循环2次即uv，要么循环3次即uvw
				for (uint32_t c = 0; c < numComponents; c++) {
					//mTextureCoords存储着所有的纹理坐标数据
					//u代表着第u个channel
					//i代表了读取第i个顶点的数据
					//c代表了第c个纹理坐标数据
					uvComponents.push_back(mesh->mTextureCoords[u][i][c]);
				}
			}
		}

		//读取当前Mesh的Index数据
		//在aimesh里面，每一个三角形都是一个Face，遍历所有的Face，将其index取出保存
		for (uint32_t f = 0; f < mesh->mNumFaces; f++)
		{
			aiFace	face = mesh->mFaces[f];

			for (uint32_t id = 0; id < face.mNumIndices; id++)
			{
				//推入每一个Face的每一个顶点的ID
				indices.push_back(face.mIndices[id]);
			}
		}

		//parse bone weights and indices, first get enought space
		skinIndices.assign(positions.size() * 4 / 3, -1.0);
		skinWeights.assign(positions.size() * 4 / 3, 0.0);

		//traverse every bone
		bool hasBone = false;

		//mNumBones对当前mesh产生影响的骨骼数量
		for (uint32_t b = 0; b < mesh->mNumBones; ++b) {
			auto aiBone = mesh->mBones[b];

			//这个骨骼的名字，也是对应了aiNode当中的某一个节点的名字
			std::string name = aiBone->mName.C_Str();

			auto bone = getBoneByName(name, bones);
			meshBones.push_back(bone);

			//todo cancel it
			offsetMatrices.push_back(getGLMMat4(aiBone->mOffsetMatrix));

			//parse  weight & indices
			//weightsNum代表了当前这跟aiBone骨骼，影响了多少个本mesh的顶点
			uint32_t weightsNum = aiBone->mNumWeights;
			auto weights = aiBone->mWeights;
			for (uint32_t w = 0; w < weightsNum; ++w) {
				//前方的解析顶点的顺序也就是当前的vertexId
				auto vertexId = weights[w].mVertexId;
				auto skinWeight = weights[w].mWeight;

				setVertexSkinData(vertexId, b, skinWeight, skinIndices, skinWeights);
			}

			hasBone = true;
		}

		//make object
		geometry->setAttribute("position", Attributef::create(positions, 3));
		geometry->setAttribute("normal", Attributef::create(normals, 3));

		//TBN
		if (tangents.size()) {
			geometry->setAttribute("tangent", Attributef::create(tangents, 3));
		}

		if (bitangents.size()) {
			geometry->setAttribute("bitangent", Attributef::create(bitangents, 3));
		}

		//skinned
		if (skinIndices.size()) {
			geometry->setAttribute("skinIndex", Attributef::create(skinIndices, 4));
			geometry->setAttribute("skinWeight", Attributef::create(skinWeights, 4));
		}

		//geometry里面是key-value，key就是attribute的名字，value就是attribute对象
		//主diffuseMap的uv仍然叫做uv
		//从第二套纹理坐标开始，都叫做uv1 uv2.....
		std::string uvName;

		//循环每一套纹理坐标
		for (uint32_t uvId = 0; uvId < uvs.size(); ++uvId) {
			uvName = "uv";
			if (uvId) {
				uvName += std::to_string(uvId);
			}
	
			//uvName : uv  uv1  uv2 uv3....。目前的引擎系统仅支持uv，还没有扩展多套uv的情况
			//取出uvs[uvId]：第uvId套uv数据vector<float>
			//numUVComponents[uvId]：第uvId套uv的itemSize，可能是2 可能是3
			geometry->setAttribute(uvName, Attributef::create(uvs[uvId], numUVComponents[uvId]));
		}

		geometry->setIndex(Attributei::create(indices, 1));

		//process material
		//从aiMesh里面，可以拿到当前Mesh所使用的Material的index，对应着aiScene->mMaterials数组的编号
		//已经解析好的materials数组跟aiScene->mMaterials是一一对应的
		if (mesh->mMaterialIndex >= 0) {
			material = materials[mesh->mMaterialIndex];
		}

		if (hasBone) {
			//这个meshBones是根据哪些骨骼对当前Mesh产生了影响，从bones数组里面抽调构建的
			auto skeleton = Skeleton::create(meshBones, offsetMatrices);
			auto skinnedMesh = SkinnedMesh::create(geometry, material);
			skinnedMesh->bind(skeleton);

			object = skinnedMesh;
			//attention: localTransform is in bone
		}
		else {
			//注意！！如果没有动画影像，则使用当前节点的Transform矩阵作为其localMatrix
			object = Mesh::create(geometry, material);
			object->setLocalMatrix(localTransform);
		}


		return object;
	}

	std::vector<AnimationClip::Ptr> AssimpLoader::processAnimation(const aiScene* scene) {
		assert(scene);

		std::vector<AnimationClip::Ptr> clips;
		//取得当前模型有多少个动画（跑动，出拳）
		uint32_t animationNum = scene->mNumAnimations;

		//遍历每一个动画进行读取
		for (uint32_t i = 0; i < animationNum; ++i) {
			auto animation = scene->mAnimations[i];
			std::string name = animation->mName.C_Str();

			//以ticks为单位的动画总时长
			float duration = animation->mDuration;

			//一秒钟有多少ticks
			float ticksPerSecond = animation->mTicksPerSecond;

			std::vector<KeyframeTrack::Ptr> tracks{};

			//遍历当前动画的所有Tracks并且构造KeyFrameTracks
			//mNumChannels是指当前的动画涉及到了多少个骨骼
			//一个Channel代表了一个骨骼（aiNode）
			for (uint32_t c = 0; c < animation->mNumChannels; ++c) {
				auto channel = animation->mChannels[c];
				std::string boneName = channel->mNodeName.C_Str();

				std::vector<float> values;
				std::vector<float> times;
				std::string trackName;

				//开始遍历当前的Channel（bone）内部的平移相关的动画关键帧
				for (uint32_t p = 0; p < channel->mNumPositionKeys; ++p) {
					auto position = getGLMVec3(channel->mPositionKeys[p].mValue);
					auto time = channel->mPositionKeys[p].mTime;

					values.push_back(position.x);
					values.push_back(position.y);
					values.push_back(position.z);

					times.push_back(time);
				}

				//如果当前确实读取到了第c个channel的position变换关键帧数据
				if (times.size()) {
					//很重要！！！这样写名字，可以用来在propertyBinding里面做正则表达式匹配，从而知道
					//当前这个keyFrameTrack对应哪个骨骼的哪个属性
					trackName = boneName + ".position";
					auto positionKeyframe = VectorKeyframeTrack::create(trackName, values, times);
					tracks.push_back(positionKeyframe);
				}

				values.clear();
				times.clear();

				//读取旋转相关关键帧
				for (uint32_t r = 0; r < channel->mNumRotationKeys; ++r) {
					auto quaternion = getGLMQuat(channel->mRotationKeys[r].mValue);
					auto time = channel->mRotationKeys[r].mTime;

					values.push_back(quaternion.x);
					values.push_back(quaternion.y);
					values.push_back(quaternion.z);
					values.push_back(quaternion.w);

					times.push_back(time);
				}

				//如果四元数的关键帧不为0
				if (times.size() != 0) {
					trackName = boneName + ".quaternion";
					auto quaternionKeyframe = QuaternionKeyframeTrack::create(trackName, values, times);
					tracks.push_back(quaternionKeyframe);
				}

				values.clear();
				times.clear();

				//读取缩放相关的关键帧
				for (uint32_t s = 0; s < channel->mNumScalingKeys; ++s) {
					auto scale = getGLMVec3(channel->mScalingKeys[s].mValue);
					auto time = channel->mScalingKeys[s].mTime;

					values.push_back(scale.x);
					values.push_back(scale.y);
					values.push_back(scale.z);

					times.push_back(time);
				}

				//如果缩放关键帧不为空
				if (times.size() != 0) {
					trackName = boneName + ".scale";
					auto scaleKeyframe = VectorKeyframeTrack::create(trackName, values, times);
					tracks.push_back(scaleKeyframe);
				}
			}

			auto clip = AnimationClip::create(name, ticksPerSecond, duration, tracks);
			clips.push_back(clip);
		}

		return clips;
	}

	void AssimpLoader::setVertexSkinData(
		const uint32_t& id,//当前第几个vertex收到影响
		const float& boneId,//当前mesh收到影响的骨骼们构成一个数组bones，这个是里面的骨骼的id号
		const float& skinWeight,//当前boneId号骨骼对当前第id个vertex产生影响的权重值
		std::vector<float>& skinIndices,
		std::vector<float>& skinWeights) {

		//系统支持，当前最多收到四根骨骼影响
		//因为当前顶点可能会收到大于四根骨骼的影响，那么先到先得
		//每个顶点就预留了四个骨骼数据的空间，填满就停下了
		for (uint32_t i = 0; i < 4; ++i) {
			if (skinIndices[id * 4 + i] < 0.0f) {
				skinIndices[id * 4 + i] = boneId;
				skinWeights[id * 4 + i] = skinWeight;
				break;
			}
		}
	}

	Bone::Ptr AssimpLoader::getBoneByName(const std::string& name, const std::vector<Bone::Ptr>& bones) noexcept {
		for (uint32_t i = 0; i < bones.size(); ++i) {
			if (bones[i]->mName == name) {
				return bones[i];
			}
		}

		return nullptr;
	}

	//系列工具函数，将向量、四元数、矩阵从assimp的格式里面转化为glm的格式
	glm::vec3 AssimpLoader::getGLMVec3(aiVector3D value) noexcept
	{
		return glm::vec3(value.x, value.y, value.z);
	}

	glm::quat AssimpLoader::getGLMQuat(aiQuaternion value) noexcept
	{
		return glm::quat(value.w, value.x, value.y, value.z);
	}

	glm::mat4 AssimpLoader::getGLMMat4(aiMatrix4x4 value) noexcept
	{
		glm::mat4 to;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		to[0][0] = value.a1; to[1][0] = value.a2; to[2][0] = value.a3; to[3][0] = value.a4;
		to[0][1] = value.b1; to[1][1] = value.b2; to[2][1] = value.b3; to[3][1] = value.b4;
		to[0][2] = value.c1; to[1][2] = value.c2; to[2][2] = value.c3; to[3][2] = value.c4;
		to[0][3] = value.d1; to[1][3] = value.d2; to[2][3] = value.d3; to[3][3] = value.d4;

		return to;
	}
}