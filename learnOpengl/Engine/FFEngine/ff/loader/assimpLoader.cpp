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

		//��ǰģ������Mesh�õ���material�����¼���������������棬˳����aiScene���
		//mMaterials��˳����ͬ
		std::vector<Material::Ptr> materials;

		//���ɸ��ڵ�
		Object3D::Ptr rootObject = Group::create();

		//�����������
		Bone::Ptr rootBone = Bone::create();
		std::vector<Bone::Ptr> bones{};

		//��ʼ���ж�ȡ
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(
			path, 
			aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "Error:model read fail!" << std::endl;
			return nullptr;
		}

		//ģ�Ͷ�ȡ��pathһ���������ģ�assets/models/superMan/man.fbx
		//ȡ������·����assets/models/superMan/
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
		//��ȡ���ж����Ĺؼ�֡���ݣ�Ϊÿ����������AnimationAction
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

	//1 ����ÿ��Node�������Mesh���ͽ�������Mesh�����Ҽ��뵽��Node��Ӧ��Group��������
	//2 ���ϵͳû�ж������ұ�Nodeû��Mesh����LocalTransform���õ���Ӧ��Group��localMatrix����
	//3 ����㼶�ܹ�
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
			//���ڵ�ǰnode�ĵ�i��Mesh��ȡ������MeshID��node->mMeshes[i]���������MeshID��Scene��������aiMesh���õ���������
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			group->addChild(processMesh(mesh, scene, getGLMMat4(node->mTransformation), materials, bones));
		}

		parentObject->addChild(group);

		//if node has meshes, then nodeTransform is judged by processMesh
		//��Ϊ�ж�����ʱ�򣬱��ڵ��mesh��WorldMatrix�任��Ӧ��������Ӱ�����Ĺ����ڵ�
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
		//ѭ������aiScene�����ÿһ��material
		for (uint32_t id = 0; id < scene->mNumMaterials; ++id) {
			Material::Ptr material{ nullptr };

			aiMaterial* aimaterial = scene->mMaterials[id];

			//������ȡ��ǰ��Material��ʲô����
			aiShadingMode shadingMode{ aiShadingMode::aiShadingMode_Phong };

			aimaterial->Get(AI_MATKEY_SHADING_MODEL, shadingMode);

			//todo: we may need more material model
			//��������ֻʵ����MeshPhongmaterial����ģ��ͨ�õĲ��ʣ�����һ��д��PhongMaterial
			switch (shadingMode) {
			case aiShadingMode::aiShadingMode_Phong:
				material = MeshPhongMaterial::create();
				break;
			default:
				material = MeshPhongMaterial::create();
				break;
			}

			//��ʼ��ȡÿ��Material����ͼ����
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
		//���ڵ�ǰģ�ͣ����ǹ�������洢��·�������ǣ�assets/models/superMan/man.fbx
		//ͼƬ���ݿ��ܻ����ڣ�assets/models/superMan/textures/�ļ�����
		//�·��õ���aiPath��Ҫ��ȡ��ͼƬ�����·���������man.fbx
		material->Get(AI_MATKEY_TEXTURE(type, 0), aiPath);

		if (!aiPath.length) {
			return nullptr;
		}

		//�е�ģ�ͣ��������һ������ģ���ڲ�����û�е�����š�
		//�鿴���ڵ�ǰ��aiPath��Ӧ��ͼƬ�������Ƿ�������ִ����ģ���ڲ������
		const aiTexture* assimpTexture = scene->GetEmbeddedTexture(aiPath.C_Str());
		if (assimpTexture) {
			//���ȷʵͼƬ�������ģ���ڲ��������������ȡ����aiTexture����ͺ�����ͼƬ����
			unsigned char* dataIn = reinterpret_cast<unsigned char*>(assimpTexture->pcData);
			uint32_t widthIn = assimpTexture->mWidth;
			uint32_t heightIn = assimpTexture->mHeight;
			std::string path = aiPath.C_Str();

			return TextureLoader::load(path, dataIn, widthIn, heightIn);
		}
		//��ΪaiPath��textures/diffuseTexture.jpg
		//ƴװ���ɣ�assets/models/superMan/textures/diffuseTexture.jpg
		std::string fullPath = rootPath + aiPath.C_Str();
		return TextureLoader::load(fullPath);
	}

	//1 �����ǰ������Mesh�յ��˹�����Ӱ�죬��ô����һ��SkinnedMesh
	//2 ����յ��˹���Ӱ�죬��ô��Attribute����Ҫ����skinIdex�Լ�skinWeight
	Object3D::Ptr AssimpLoader::processMesh(
		const aiMesh* mesh,
		const aiScene* scene,
		//��ǰMesh������Node���������һ��mesh�����޷�����LocalMatrix
		const glm::mat4 localTransform,
		const std::vector<Material::Ptr>& materials,
		const std::vector<Bone::Ptr>& bones) {

		Object3D::Ptr object = nullptr;
		Material::Ptr material = nullptr;
		Geometry::Ptr geometry = Geometry::create();

		std::vector<Bone::Ptr> meshBones;//�Ե�ǰMesh����Ӱ���bone������
		std::vector<glm::mat4> offsetMatrices;//��Ӧ���������ÿһ��bone��offsetMatrix

		//һ��Mesh����Ҫ������attributes
		std::vector<float> positions;
		std::vector<float> normals;
		std::vector<float> tangents;
		std::vector<float> bitangents;

		std::vector<std::vector<float>> uvs;//ÿһ��Ԫ�ض��ǵ�i��channel��������������
		std::vector<uint32_t> numUVComponents;//ÿһ��Ԫ�ض��ǵ�i��channel��itemSize

		std::vector<uint32_t> indices;
		std::vector<float> skinIndices;
		std::vector<float> skinWeights;

		//���ն�����������
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
			//1 һ��ģ�Ϳ������е�mesh���ж����ͼ����ͼ���ܻ�����ͬ���ܣ�����DiffuseMap�����ж��ţ���ô��ɫ�ͻ���
			//Ҳ�п����ǲ�ͬ���ܣ�����NormalMap SpecularMap DiffuseMap��
			// 2 ��Ȼ�ж�����ͼ���ܣ�ͬһ�����������ͬ���������ܻ��в�ͬ��uv����
			// 3 ����������в�ͬ���ͣ����������λͼƬ�����Ǽ򵥵Ķ�λuv���������������ͼuvw��str��
			// 
			// �ܽ᣺����ͬһ��Mesh����ȡ��ĳ�������uv
			// 1 ���ж���uv���ֲ��ڲ�ͬ��Channel
			// 2 ��ȡ���������ʱ��Ҫ�ж���uv����uvw
			//

			//GetNumUVChannels:��ȡ��ǰMesh�ж�������������
			//����ͬ��Channel��������������˲�ͬ��vector<float>����
			for (uint32_t u = 0; u < mesh->GetNumUVChannels(); ++u) {
				if (u >= uvs.size()) {
					uvs.push_back(std::vector<float>());
				}
				std::vector<float>& uvComponents = uvs[u];

				//�鿴���ڵ�ǰ���Channel������������uv����uvw
				//mNumUVComponents �洢�˵�ǰ��u��Channel����Ӧ����һ����������itemSize
				uint32_t numComponents = mesh->mNumUVComponents[u];

				//uv  or  uvw ��������ԭ�����������ǰ������������Ϊattribute����geometry���ڹ���
				//attribute��ʱ�򣬾���Ҫ֪��itemSize
				if (u >= numUVComponents.size()) {
					numUVComponents.push_back(numComponents);
				}

				//����numComponents���б�����ȡ��Ҫôѭ��2�μ�uv��Ҫôѭ��3�μ�uvw
				for (uint32_t c = 0; c < numComponents; c++) {
					//mTextureCoords�洢�����е�������������
					//u�����ŵ�u��channel
					//i�����˶�ȡ��i�����������
					//c�����˵�c��������������
					uvComponents.push_back(mesh->mTextureCoords[u][i][c]);
				}
			}
		}

		//��ȡ��ǰMesh��Index����
		//��aimesh���棬ÿһ�������ζ���һ��Face���������е�Face������indexȡ������
		for (uint32_t f = 0; f < mesh->mNumFaces; f++)
		{
			aiFace	face = mesh->mFaces[f];

			for (uint32_t id = 0; id < face.mNumIndices; id++)
			{
				//����ÿһ��Face��ÿһ�������ID
				indices.push_back(face.mIndices[id]);
			}
		}

		//parse bone weights and indices, first get enought space
		skinIndices.assign(positions.size() * 4 / 3, -1.0);
		skinWeights.assign(positions.size() * 4 / 3, 0.0);

		//traverse every bone
		bool hasBone = false;

		//mNumBones�Ե�ǰmesh����Ӱ��Ĺ�������
		for (uint32_t b = 0; b < mesh->mNumBones; ++b) {
			auto aiBone = mesh->mBones[b];

			//������������֣�Ҳ�Ƕ�Ӧ��aiNode���е�ĳһ���ڵ������
			std::string name = aiBone->mName.C_Str();

			auto bone = getBoneByName(name, bones);
			meshBones.push_back(bone);

			//todo cancel it
			offsetMatrices.push_back(getGLMMat4(aiBone->mOffsetMatrix));

			//parse  weight & indices
			//weightsNum�����˵�ǰ���aiBone������Ӱ���˶��ٸ���mesh�Ķ���
			uint32_t weightsNum = aiBone->mNumWeights;
			auto weights = aiBone->mWeights;
			for (uint32_t w = 0; w < weightsNum; ++w) {
				//ǰ���Ľ��������˳��Ҳ���ǵ�ǰ��vertexId
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

		//geometry������key-value��key����attribute�����֣�value����attribute����
		//��diffuseMap��uv��Ȼ����uv
		//�ӵڶ����������꿪ʼ��������uv1 uv2.....
		std::string uvName;

		//ѭ��ÿһ����������
		for (uint32_t uvId = 0; uvId < uvs.size(); ++uvId) {
			uvName = "uv";
			if (uvId) {
				uvName += std::to_string(uvId);
			}
	
			//uvName : uv  uv1  uv2 uv3....��Ŀǰ������ϵͳ��֧��uv����û����չ����uv�����
			//ȡ��uvs[uvId]����uvId��uv����vector<float>
			//numUVComponents[uvId]����uvId��uv��itemSize��������2 ������3
			geometry->setAttribute(uvName, Attributef::create(uvs[uvId], numUVComponents[uvId]));
		}

		geometry->setIndex(Attributei::create(indices, 1));

		//process material
		//��aiMesh���棬�����õ���ǰMesh��ʹ�õ�Material��index����Ӧ��aiScene->mMaterials����ı��
		//�Ѿ������õ�materials�����aiScene->mMaterials��һһ��Ӧ��
		if (mesh->mMaterialIndex >= 0) {
			material = materials[mesh->mMaterialIndex];
		}

		if (hasBone) {
			//���meshBones�Ǹ�����Щ�����Ե�ǰMesh������Ӱ�죬��bones����������������
			auto skeleton = Skeleton::create(meshBones, offsetMatrices);
			auto skinnedMesh = SkinnedMesh::create(geometry, material);
			skinnedMesh->bind(skeleton);

			object = skinnedMesh;
			//attention: localTransform is in bone
		}
		else {
			//ע�⣡�����û�ж���Ӱ����ʹ�õ�ǰ�ڵ��Transform������Ϊ��localMatrix
			object = Mesh::create(geometry, material);
			object->setLocalMatrix(localTransform);
		}


		return object;
	}

	std::vector<AnimationClip::Ptr> AssimpLoader::processAnimation(const aiScene* scene) {
		assert(scene);

		std::vector<AnimationClip::Ptr> clips;
		//ȡ�õ�ǰģ���ж��ٸ��������ܶ�����ȭ��
		uint32_t animationNum = scene->mNumAnimations;

		//����ÿһ���������ж�ȡ
		for (uint32_t i = 0; i < animationNum; ++i) {
			auto animation = scene->mAnimations[i];
			std::string name = animation->mName.C_Str();

			//��ticksΪ��λ�Ķ�����ʱ��
			float duration = animation->mDuration;

			//һ�����ж���ticks
			float ticksPerSecond = animation->mTicksPerSecond;

			std::vector<KeyframeTrack::Ptr> tracks{};

			//������ǰ����������Tracks���ҹ���KeyFrameTracks
			//mNumChannels��ָ��ǰ�Ķ����漰���˶��ٸ�����
			//һ��Channel������һ��������aiNode��
			for (uint32_t c = 0; c < animation->mNumChannels; ++c) {
				auto channel = animation->mChannels[c];
				std::string boneName = channel->mNodeName.C_Str();

				std::vector<float> values;
				std::vector<float> times;
				std::string trackName;

				//��ʼ������ǰ��Channel��bone���ڲ���ƽ����صĶ����ؼ�֡
				for (uint32_t p = 0; p < channel->mNumPositionKeys; ++p) {
					auto position = getGLMVec3(channel->mPositionKeys[p].mValue);
					auto time = channel->mPositionKeys[p].mTime;

					values.push_back(position.x);
					values.push_back(position.y);
					values.push_back(position.z);

					times.push_back(time);
				}

				//�����ǰȷʵ��ȡ���˵�c��channel��position�任�ؼ�֡����
				if (times.size()) {
					//����Ҫ����������д���֣�����������propertyBinding������������ʽƥ�䣬�Ӷ�֪��
					//��ǰ���keyFrameTrack��Ӧ�ĸ��������ĸ�����
					trackName = boneName + ".position";
					auto positionKeyframe = VectorKeyframeTrack::create(trackName, values, times);
					tracks.push_back(positionKeyframe);
				}

				values.clear();
				times.clear();

				//��ȡ��ת��عؼ�֡
				for (uint32_t r = 0; r < channel->mNumRotationKeys; ++r) {
					auto quaternion = getGLMQuat(channel->mRotationKeys[r].mValue);
					auto time = channel->mRotationKeys[r].mTime;

					values.push_back(quaternion.x);
					values.push_back(quaternion.y);
					values.push_back(quaternion.z);
					values.push_back(quaternion.w);

					times.push_back(time);
				}

				//�����Ԫ���Ĺؼ�֡��Ϊ0
				if (times.size() != 0) {
					trackName = boneName + ".quaternion";
					auto quaternionKeyframe = QuaternionKeyframeTrack::create(trackName, values, times);
					tracks.push_back(quaternionKeyframe);
				}

				values.clear();
				times.clear();

				//��ȡ������صĹؼ�֡
				for (uint32_t s = 0; s < channel->mNumScalingKeys; ++s) {
					auto scale = getGLMVec3(channel->mScalingKeys[s].mValue);
					auto time = channel->mScalingKeys[s].mTime;

					values.push_back(scale.x);
					values.push_back(scale.y);
					values.push_back(scale.z);

					times.push_back(time);
				}

				//������Źؼ�֡��Ϊ��
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
		const uint32_t& id,//��ǰ�ڼ���vertex�յ�Ӱ��
		const float& boneId,//��ǰmesh�յ�Ӱ��Ĺ����ǹ���һ������bones�����������Ĺ�����id��
		const float& skinWeight,//��ǰboneId�Ź����Ե�ǰ��id��vertex����Ӱ���Ȩ��ֵ
		std::vector<float>& skinIndices,
		std::vector<float>& skinWeights) {

		//ϵͳ֧�֣���ǰ����յ��ĸ�����Ӱ��
		//��Ϊ��ǰ������ܻ��յ������ĸ�������Ӱ�죬��ô�ȵ��ȵ�
		//ÿ�������Ԥ�����ĸ��������ݵĿռ䣬������ͣ����
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

	//ϵ�й��ߺ���������������Ԫ���������assimp�ĸ�ʽ����ת��Ϊglm�ĸ�ʽ
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