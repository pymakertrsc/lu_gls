#include "../ff/core/attribute.h"
#include "../ff/core/geometry.h"
#include "../ff/objects/mesh.h"
#include "../ff/scene/scene.h"
#include "../ff/camera/perspectiveCamera.h"
#include "../ff/cameraControl/gameCameraControl.h"
#include "../ff/render/renderer.h"
#include "../ff/material/meshPhongMaterial.h"
#include "../ff/log/debugLog.h"
#include "../ff/global/constant.h"
#include "../ff/geometries/boxGeometry.h"
#include "../ff/textures/texture.h"
#include "../ff/loader/textureLoader.h"
#include "../ff/lights/directionalLight.h"
#include "../ff/lights/ambientLight.h"


uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;

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
	auto boxGeometry = ff::BoxGeometry::create(1.0, 1.0, 1.0);

	auto material = ff::MeshPhongMaterial::create();
	material->mDiffuseMap = ff::TextureLoader::load("assets/textures/cat.png");

	cube = ff::Mesh::create(boxGeometry, material);

	auto directionalLight = ff::DirectionalLight::create();
	directionalLight->setPosition(0.0f, 0.0f, 4.0f);
	directionalLight->mColor = glm::vec3(1.0f, 1.0f, 1.0f);
	directionalLight->mIntensity = 1.0;
	directionalLight->lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	auto ambientLight = ff::AmbientLight::create();
	ambientLight->mColor = glm::vec3(1.0f, 0.0f, 0.0f);
	ambientLight->mIntensity = 0.2;


	auto scene = ff::Scene::create();
	scene->addChild(cube);
	scene->addChild(directionalLight);
	scene->addChild(ambientLight);

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

	while (true) {
		if (!renderer->render(scene, camera)) {
			break;
		}

		renderer->swap();

		rotateCube();
	}

}