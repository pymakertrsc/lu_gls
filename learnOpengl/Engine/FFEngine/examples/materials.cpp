#include "../ff/core/attribute.h"
#include "../ff/core/geometry.h"
#include "../ff/objects/mesh.h"
#include "../ff/scene/scene.h"
#include "../ff/camera/perspectiveCamera.h"
#include "../ff/cameraControl/gameCameraControl.h"
#include "../ff/render/renderer.h"
#include "../ff/material/meshBasicMaterial.h"
#include "../ff/material/meshPhongMaterial.h"
#include "../ff/material/depthMaterial.h"
#include "../ff/log/debugLog.h"
#include "../ff/global/constant.h"
#include "../ff/geometries/boxGeometry.h"
#include "../ff/textures/texture.h"
#include "../ff/loader/textureLoader.h"
#include "../ff/lights/directionalLight.h"


uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;

ff::Mesh::Ptr meshBasicCube = nullptr;
ff::Mesh::Ptr meshPhongCube = nullptr;
ff::Mesh::Ptr meshDepthCube = nullptr;
ff::Mesh::Ptr meshTransparentCube = nullptr;


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
	meshBasicCube->rotateAroundAxis(glm::vec3(1.0, 1.0, 1.0), angle);
	meshPhongCube->rotateAroundAxis(glm::vec3(1.0, 1.0, 1.0), angle);
	meshDepthCube->rotateAroundAxis(glm::vec3(1.0, 1.0, 1.0), angle);
	meshTransparentCube->rotateAroundAxis(glm::vec3(1.0, 1.0, 1.0), angle);
}

int main() {
	auto boxGeometry = ff::BoxGeometry::create(1.0, 1.0, 1.0);

	auto meshBasicMaterial = ff::MeshBasicMaterial::create();
	meshBasicMaterial->mDiffuseMap = ff::TextureLoader::load("assets/textures/bing.png");
	meshBasicCube = ff::Mesh::create(boxGeometry, meshBasicMaterial);
	meshBasicCube->setPosition(-2.0, 0.0, 0.0);

	//与光照有反应
	auto meshPhongMaterial = ff::MeshPhongMaterial::create();
	meshPhongMaterial->mDiffuseMap = ff::TextureLoader::load("assets/textures/stars.jpg");
	meshPhongCube = ff::Mesh::create(boxGeometry, meshPhongMaterial);
	meshPhongCube->setPosition(0.0, 0.0, 0.0);

	//用来将物体的每个Fragment的深度值渲染在屏幕上
	auto depthMaterial = ff::DepthMaterial::create();
	meshDepthCube = ff::Mesh::create(boxGeometry, depthMaterial);
	meshDepthCube->setPosition(0.0, 0.0, -2.0);

	//用的还是基础材质，只不过启用了透明度
	auto meshTransparentMaterial = ff::MeshBasicMaterial::create();
	meshTransparentMaterial->mDiffuseMap = ff::TextureLoader::load("assets/textures/bing.jpg"); 
	meshTransparentMaterial->mTransparent = true;
	meshTransparentMaterial->mOpacity = 0.4;
	meshTransparentCube = ff::Mesh::create(boxGeometry, meshTransparentMaterial);
	meshTransparentCube->setPosition(1.0, 0.0, 1.0);

	//创建平行光
	auto directionalLight = ff::DirectionalLight::create();
	directionalLight->setPosition(4.0f, 0.0f, 4.0f);
	directionalLight->mColor = glm::vec3(1.0f, 1.0f, 1.0f);
	directionalLight->mIntensity = 1.0;
	directionalLight->lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	auto scene = ff::Scene::create();
	scene->addChild(meshBasicCube);
	scene->addChild(meshPhongCube);
	scene->addChild(meshDepthCube);
	scene->addChild(meshTransparentCube);

	//重要，光也是要加入到场景内部的
	scene->addChild(directionalLight);

	auto camera = ff::PerspectiveCamera::create(0.1f, 100.0f, (float)WIDTH / (float)(HEIGHT), 60.0f);
	camera->setPosition(2.0f, 2.0f, 2.0f);

	//up是指大方向的世界up，只要你没倒立过来，就是(0 , 1 , 0)
	camera->lookAt(glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

	ff::Renderer::Descriptor rDc;
	rDc.mWidth = WIDTH;
	rDc.mHeight = HEIGHT;
	ff::Renderer::Ptr renderer = ff::Renderer::create(rDc);
	renderer->setClearColor(0.4, 0.5, 0.4, 1.0);

	renderer->setMouseActionCallback(onMouseAction);
	renderer->setKeyboardActionCallBack(onKeyboardAction);
	renderer->setFrameSizeCallBack(onResize);
	renderer->setMouseMoveCallBack(onMouseMove);

	while (true) {
		if (!renderer->render(scene, camera)) {
			break;
		}

		renderer->swap();

		rotateCube();
	}

}