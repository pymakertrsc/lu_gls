#pragma once
#include "../global/base.h"
#include "../math/math.h"

#define NO_MOVE 0
#define MOVE_LEFT 0x001
#define MOVE_RIGHT 0x002
#define MOVE_FRONT 0x004
#define MOVE_BACK 0x008

//1 接收到外部操作（鼠标/键盘）
//2 根据操作更改当前状态（移动状态/front向量）
//3 每一帧由用户主动调用update，更新viewMatrix矩阵

class Camera {
public:
	Camera(float fovy, float aspect, float n, float f, const math::vec3f& top);

	~Camera();

public:
	void onRMouseDown(const int& x, const int& y);

	void onRMouseUp(const int& x, const int& y);

	void onMouseMove(const int& x, const int& y);

	void onKeyDown(const uint32_t& key);

	void onKeyUp(const uint32_t& key);

	void update();

	math::mat4f getViewMatrix()const { return mViewMatrix; }

	math::mat4f getProjectionMatrix()const { return mProjectionMatrix; }

private:
	void pitch(int yoffset);

	void yaw(int xoffset);

private:
	math::mat4f mViewMatrix;
	math::mat4f mProjectionMatrix;

	math::vec3f	mPosition{ 0.0f,0.0f,0.0f };
	math::vec3f	mFront{ 0.0f,0.0f,-1.0f };
	math::vec3f	mTop{ 0.0f,1.0f,0.0f };
	float		mSpeed = { 0.1f };

	float		mPitch{ 0.0f };
	float		mYaw{ -90.0f };
	float		mSensitivity{ 0.1f };

	uint32_t	mMoveState{ NO_MOVE };
	bool		mMouseMoving{ false };
	int			mCurrentMouseX{ 0 };
	int			mCurrentMouseY{ 0 };

};