#include "../ff/core/attribute.h"
#include "../ff/core/geometry.h"
#include "../ff/objects/mesh.h"
#include "../ff/scene/scene.h"
#include "../ff/camera/perspectiveCamera.h"
#include "../ff/cameraControl/gameCameraControl.h"
#include "../ff/render/renderer.h"
#include "../ff/material/meshBasicMaterial.h"
#include "../ff/log/debugLog.h"
#include "../ff/global/constant.h"
#include "../ff/geometries/boxGeometry.h"
#include "../ff/textures/texture.h"
#include "../ff/loader/textureLoader.h"
#include "../ff/loader/cubeTextureLoader.h"
#include "../ff/wrapper/glWrapper.hpp"

uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;

//mesh类型的智能指针
ff::Mesh::Ptr cube = nullptr;

static void onMouseMove(double xpos, double ypos) {
}

static void onMouseAction(ff::MouseAction action) {
}

static void onKeyboardAction(KeyBoardState action) {
}

static void onResize(int width, int height) {
}

float angle = 0.6f;
void rotateCube() {
	cube->rotateAroundAxis(glm::vec3(1.0, 1.0, 1.0), angle);
}

int main() {

	//创建Cube的Geometry
	auto boxGeometry = ff::BoxGeometry::create(1.0, 1.0, 1.0);
	
	//创建基础材质
	auto material = ff::MeshBasicMaterial::create();
	auto diffuseMap = ff::TextureLoader::load("assets/textures/cat.png");
	material->mDiffuseMap = diffuseMap;

	cube = ff::Mesh::create(boxGeometry, material);

	auto scene = ff::Scene::create();
	scene->addChild(cube);

	//sky box
	std::vector<std::string> cubePaths = {
		"assets/textures/skybox/right.jpg",
		"assets/textures/skybox/left.jpg",
		"assets/textures/skybox/top.jpg",
		"assets/textures/skybox/bottom.jpg",
		"assets/textures/skybox/front.jpg",
		"assets/textures/skybox/back.jpg",
	};

	ff::CubeTexture::Ptr cubeTexture = ff::CubeTextureLoader::load(cubePaths);
	scene->mBackground = cubeTexture;

	auto camera = ff::PerspectiveCamera::create(0.1f, 100.0f, (float)WIDTH / (float)(HEIGHT), 60.0f);
	camera->setPosition(0.0f, 0.0f, 2.0f);

	ff::Renderer::Descriptor rDc;
	rDc.mWidth = WIDTH;
	rDc.mHeight = HEIGHT;
	ff::Renderer::Ptr renderer = ff::Renderer::create(rDc);
	renderer->setClearColor(0.94, 1.0, 0.94, 1.0);

	renderer->setMouseActionCallback(onMouseAction);
	renderer->setKeyboardActionCallBack(onKeyboardAction);
	renderer->setFrameSizeCallBack(onResize);
	renderer->setMouseMoveCallBack(onMouseMove);

	ff::RenderTarget::Options options;
	ff::RenderTarget::Ptr renderTarget = ff::RenderTarget::create(WIDTH, HEIGHT, options);

	while (true) {
		//第一个pass,渲染立方体
		material->mDiffuseMap = diffuseMap;
		renderer->setRenderTarget(renderTarget);
		if (!renderer->render(scene, camera)) {
			break;
		}

		//第二个pass，将第一个pass渲染的结果，作为diffuseMap输送给了当前cube使用的material
		material->mDiffuseMap = renderTarget->getTexture();

		//改为向默认的renderatarget进行渲染
		renderer->setRenderTarget(nullptr);
		if (!renderer->render(scene, camera)) {
			break;
		}

		renderer->swap();

		rotateCube();
	}

}