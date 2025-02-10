#pragma once
#include "../shader.h"
#include "../../math/math.h"

struct DirectionalLight {
	math::vec3f color;
	math::vec3f direction;
};

class LightShader :public Shader {
public:
	LightShader();
	~LightShader();

	VsOutput vertexShader(
		const std::map<uint32_t, BindingDescription>& bindingMap,
		const std::map<uint32_t, BufferObject*>& bufferMap,
		const uint32_t& index
	)override;

	void fragmentShader(const VsOutput& input, FsOutput& output, const std::map<uint32_t, Texture*>& textures)override;

public:
	//uniforms
	math::mat4f mModelMatrix;
	math::mat4f mViewMatrix;
	math::mat4f mProjectionMatrix;

	uint32_t mDiffuseTexture{ 0 };
	DirectionalLight mDirectionalLight;
	math::vec3f mEnvLight;
};