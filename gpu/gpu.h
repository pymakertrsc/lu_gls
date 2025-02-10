#pragma once
#include "../global/base.h"
#include "frameBuffer.h"
#include "../application/application.h"
#include "../math/math.h"
#include "dataStructures.h"
#include "bufferObject.h"
#include "vao.h"
#include "shader.h"
#include "texture.h"

#define sgl GPU::getInstance()

/*
* class GPU：
* 模拟GPU的绘图行为以及算法等
*/
class GPU {
public:
	static GPU* getInstance();
	GPU();

	~GPU();

	//接受外界传入的bmp对应的内存指针以及窗体的宽/高
	void initSurface(const uint32_t& width, const uint32_t& height, void* buffer = nullptr);

	//清除画布内容
	void clear();

	//打印状态机
	void printVAO(const uint32_t& vaoID);

	uint32_t genBuffer();
	void deleteBuffer(const uint32_t& bufferID);
	void bindBuffer(const uint32_t& bufferType, const uint32_t& bufferID);
	void bufferData(const uint32_t& bufferType, size_t dataSize, void* data);

	uint32_t genVertexArray();
	void deleteVertexArray(const uint32_t& vaoID);
	void bindVertexArray(const uint32_t& vaoID);
	void vertexAttributePointer(
		const uint32_t& binding,
		const uint32_t& itemSize,
		const uint32_t& stride,
		const uint32_t& offset);

	void useProgram(Shader* shader);

	void enable(const uint32_t& value);
	void disable(const uint32_t& value);

	//cull face
	void frontFace(const uint32_t& value);
	void cullFace(const uint32_t& value);

	//depth test
	void depthFunc(const uint32_t& depthFunc);
	void depthWrite(bool value);

	//textures
	uint32_t genTexture();
	void deleteTexture(const uint32_t& texID);
	void bindTexture(const uint32_t& texID);
	void texImage2D(const uint32_t& width, const uint32_t& height, void* data);
	void texParameter(const uint32_t& param, const uint32_t& value);

	void drawElement(const uint32_t& drawMode, const uint32_t& first, const uint32_t& count);

private:
	void vertexShaderStage(
		std::vector<VsOutput>& vsOutputs,
		const VertexArrayObject* vao,
		const BufferObject* ebo,
		const uint32_t first,
		const uint32_t count);

	void perspectiveDivision(VsOutput& vsOutput);
	void perspectiveRecover(VsOutput& vsOutput);
	void screenMapping(VsOutput& vsOutput);

	void trim(VsOutput& vsOutput);

	bool depthTest(const FsOutput& output);

	RGBA blend(const FsOutput& output);

private:
	static GPU* mInstance;
	FrameBuffer* mFrameBuffer{ nullptr };

	//VBO相关/EBO也存在内部
	uint32_t mCurrentVBO{ 0 };
	uint32_t mCurrentEBO{ 0 };
	uint32_t mBufferCounter{ 0 };
	std::map<uint32_t, BufferObject*> mBufferMap;

	//VAO相关
	uint32_t mCurrentVAO{ 0 };
	uint32_t mVaoCounter{ 0 };
	std::map<uint32_t, VertexArrayObject*> mVaoMap;

	Shader* mShader{ nullptr };
	math::mat4f mScreenMatrix;

	//cull face
	bool mEnableCullFace{ false };
	uint32_t mFrontFace{ FRONT_FACE_CCW };
	uint32_t mCullFace{ BACK_FACE };

	//depth
	bool mEnableDepthTest{ true };
	bool mEnableDepthWrite{ true };
	uint32_t mDepthFunc{ DEPTH_LESS };

	//blending
	bool mEnableBlending{ false };

	//texture
	uint32_t mCurrentTexture{ 0 };
	uint32_t mTextureCounter{ 0 };
	std::map<uint32_t, Texture*> mTextureMap;
};