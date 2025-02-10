#pragma once
#include "../global/base.h"
#include "../math/math.h"
#include "image.h"
#include "../gpu/shader/lightShader.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

struct Vertex {
	math::vec3f position;
	math::vec3f normal;
	math::vec2f texCoords;
};

class Model;
class Mesh {
public:
	Mesh(
		const std::vector<Vertex>& vertices, 
		const std::vector<unsigned int> indices, 
		uint32_t diffuseTexture,
		const math::mat4f& localMatrix
	);

	~Mesh();

	void addChild(Mesh* mesh);

	void draw(const math::mat4f& preMatrix, LightShader* shader);

private:
	friend class Model;
	uint32_t mVao{ 0 };
	uint32_t mVbo{ 0 };
	uint32_t mEbo{ 0 };
	uint32_t mTexture{ 0 };
	uint32_t mIndicesCount{ 0 };

	math::mat4f mLocalMatrix;

	std::vector<Mesh*> mChildren;
};

class Model {
public:
	Model();
	~Model();

	void read(const std::string& path);

	void draw(LightShader* shader);

	void setModelMatrix(const math::mat4f& m);

private:
	void processNode(Mesh* parent, aiNode* ainode, const aiScene* scene);
	
	Mesh* processMesh(aiMesh* aimesh, const aiScene* scene);

	uint32_t processTexture(
		const aiMaterial* material,
		const aiTextureType& type,
		const aiScene* scene
	);

	uint32_t createTexture(Image* image);

	static math::mat4f getMat4f(aiMatrix4x4 value);

private:
	std::string mRootPath{};
	Mesh* mRootMesh{ nullptr };
	std::vector<Mesh*> mMeshes;

	//image path and textureID
	std::map<std::string, uint32_t> mTextureCache;
};
