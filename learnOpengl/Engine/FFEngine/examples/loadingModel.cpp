#include "../ff/global/base.h"
#include "../ff/core/attribute.h"
#include "../ff/core/geometry.h"
#include "../ff/core/object3D.h"
#include "../ff/objects/mesh.h"
#include "../ff/scene/scene.h"
#include "../ff/camera/perspectiveCamera.h"
#include "../ff/camera/orthographicCamera.h"
#include "../ff/cameraControl/gameCameraControl.h"
#include "../ff/render/renderer.h"
#include "../ff/material/meshBasicMaterial.h"
#include "../ff/material/meshPhongMaterial.h"
#include "../ff/material/depthMaterial.h"
#include "../ff/loader/textureLoader.h"
#include "../ff/loader/cubeTextureLoader.h"
#include "../ff/geometries/boxGeometry.h"
#include "../ff/lights/directionalLight.h"
#include "../ff/lights/ambientLight.h"
#include "../ff/render/renderTarget.h"
#include "../ff/geometries/planeGeometry.h"
#include "../ff/tools/timer.h"
#include "../ff/math/interpolants/linearInterpolant.h"
#include "../ff/loader/assimpLoader.h"
#include "../ff/log/debugLog.h"

using namespace ff;

Scene::Ptr	scene = nullptr;
DirectionalLight::Ptr directionalLight = nullptr;
Camera::Ptr camera = nullptr;
GameCameraControl::Ptr cameraControl = nullptr;


static void onMouseMove(double xpos, double ypos) {
	cameraControl->onMouseMove(xpos, ypos);
}

static void onMouseAction(MouseAction action) {
	cameraControl->onMouseAction(action);
}

static void onKeyboardAction(KeyBoardState action) {
	cameraControl->onKeyboard(action);
}

void onResize(int width, int height) {
}

Scene::Ptr makeScene() {

	Scene::Ptr scene = Scene::create();

	//sky box
	std::vector<std::string> cubePaths = {
		"assets/textures/skybox/right.jpg",
		"assets/textures/skybox/left.jpg",
		"assets/textures/skybox/top.jpg",
		"assets/textures/skybox/bottom.jpg",
		"assets/textures/skybox/front.jpg",
		"assets/textures/skybox/back.jpg",
	};

	CubeTexture::Ptr cubeTexture = CubeTextureLoader::load(cubePaths);
	scene->mBackground = cubeTexture;

	//lights
	directionalLight = DirectionalLight::create();
	directionalLight->setPosition(4.0f, 4.0f, 4.0f);
	directionalLight->mColor = glm::vec3(1.0f, 1.0f, 1.0f);
	directionalLight->mIntensity = 1.0;
	directionalLight->lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	
	//本灯光会产生阴影
	directionalLight->mCastShadow = true;
	directionalLight->mShadow->mMapSize = glm::vec2(2048.0, 2048.0);
	directionalLight->mShadow->mCamera = OrthographicCamera::create(-90, 90, -90, 90, -90, 90);


	auto ambientLight = AmbientLight::create();
	ambientLight->mIntensity = 0.2f;

	scene->addChild(directionalLight);
	scene->addChild(ambientLight);

	auto model = AssimpLoader::load("assets/models/tian/tian.fbx");
	model->mObject->setPosition(0.0, 0.0, 0.0);
	model->mObject->rotateX(-90.0);
	model->mObject->setScale(0.01, 0.01, 0.01);
	scene->addChild(model->mObject);

	return scene;
}

int main() {
	try {
		scene = makeScene();

		Renderer::Descriptor rDc;
		rDc.mWidth = 1200;
		rDc.mHeight = 800;

		camera = PerspectiveCamera::create(0.1, 10000, static_cast<float>(rDc.mWidth) / static_cast<float>(rDc.mHeight), 45.0f);
		camera->setPosition(3, 4, 15);

		cameraControl = GameCameraControl::create(camera);
		cameraControl->setSpeed(0.2f);

		//生成renderer
		Renderer::Ptr renderer = Renderer::create(rDc);
		renderer->setMouseActionCallback(onMouseAction);
		renderer->setKeyboardActionCallBack(onKeyboardAction);
		renderer->setFrameSizeCallBack(onResize);
		renderer->setMouseMoveCallBack(onMouseMove);

		while (true) {
			cameraControl->update();

			if (!renderer->render(scene, camera)) {
				break;
			}

			renderer->swap();

		}
	}
	catch (std::exception e) {
		std::cout << e.what() << std::endl;
	}


	return 0;
}