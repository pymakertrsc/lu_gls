#pragma once
#include "../shader.h"
#include "../../math/math.h"

class TextureShader :public Shader {
public:
	TextureShader();
	~TextureShader();

	VsOutput vertexShader(
		const std::map<uint32_t, BindingDescription>& bindingMap,
		const std::map<uint32_t, BufferObject*>& bufferMap,
		const uint32_t& index
	)override;

	void fragmentShader(const VsOutput& input, FsOutput& output, const std::map<uint32_t, Texture*>& textures)override;

public:
	uint32_t mDiffuseTexture{ 0 };
};