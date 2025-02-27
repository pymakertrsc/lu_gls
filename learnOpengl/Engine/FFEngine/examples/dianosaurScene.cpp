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
Mesh::Ptr	mesh = nullptr;
Mesh::Ptr	phongMesh = nullptr;
Mesh::Ptr	depthMesh = nullptr;
Mesh::Ptr	plane = nullptr;
DirectionalLight::Ptr directionalLight0 = nullptr;
DirectionalLight::Ptr directionalLight1 = nullptr;
DirectionalLight::Ptr directionalLight2 = nullptr;
RenderTarget::Ptr renderTarget = nullptr;
Camera::Ptr camera = nullptr;
GameCameraControl::Ptr cameraControl = nullptr;

AssimpResult::Ptr dragon = nullptr;

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

float t = 0.0;
float radius = 5.0f;
void rotateLight() {

	glm::vec3 position;
	position.y = 5.0f;
	position.x = std::cos(t) * radius;
	position.z = std::sin(t) * radius;


	directionalLight0->setPosition(position);

	directionalLight0->lookAt(glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

	t += 0.01f;
}

Scene::Ptr makeScene() {


	Geometry::Ptr geometry = Geometry::create();

	std::vector<float> positionData = {
		-1.0, -1.0,  1.0,
		1.0, -1.0,  1.0,
		1.0,  1.0,  1.0,
	};

	std::vector<float> normalData = {
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f
	};

	std::vector<float> colorData = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};

	std::vector<float> uvData = {
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f
	};

	std::vector<uint32_t> indices = {
		0, 1, 2
	};

	geometry->setAttribute("position", Attributef::create(positionData, 3));
	geometry->setAttribute("normal", Attributef::create(normalData, 3));
	geometry->setAttribute("color", Attributef::create(colorData, 3));
	geometry->setAttribute("uv", Attributef::create(uvData, 2));
	geometry->setIndex(Attributei::create(indices, 1));

	geometry = BoxGeometry::create(0.5, 0.6, 0.5);

	auto texture = TextureLoader::load("assets/textures/wall.jpg");

	MeshBasicMaterial::Ptr material = MeshBasicMaterial::create();
	material->mFrontFace = FrontFace::FrontCounterClockWise;
	material->mSide = Side::DoubleSide;
	material->mDiffuseMap = texture;
	material->mDepthWrite = true;
	material->mDrawMode = DrawMode::Triangles;

	mesh = Mesh::create(geometry, material);
	mesh->setPosition(0.0f, 0.5f, 0.6f);
	mesh->mCastShadow = true;

	MeshPhongMaterial::Ptr phongMaterial = MeshPhongMaterial::create();
	phongMaterial->mDiffuseMap = texture;
	phongMaterial->mSide = Side::FrontSide;

	phongMesh = Mesh::create(geometry, phongMaterial);
	phongMesh->mCastShadow = true;

	DepthMaterial::Ptr depthMaterial = DepthMaterial::create();
	depthMaterial->mPacking = DepthMaterial::RGBADepthPacking;

	depthMesh = Mesh::create(geometry, depthMaterial);
	depthMesh->setPosition(0.0f, 0.0f, 0.0f);

	Scene::Ptr scene = Scene::create();


	auto planeGeometry = PlaneGeometry::create(8.0, 8.0, 1, 1);
	plane = Mesh::create(planeGeometry, phongMaterial);
	plane->rotateX(-90.0f);
	plane->setPosition(0.0, -1.0, 0.0);

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
	float projectSize = 14.0;
	directionalLight0 = DirectionalLight::create();
	directionalLight0->setPosition(4.0f, 4.0f, 4.0f);
	directionalLight0->mColor = glm::vec3(1.0f, 1.0f, 1.0f);
	directionalLight0->mIntensity = 1.0;
	directionalLight0->lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	directionalLight0->mCastShadow = true;
	directionalLight0->mShadow->mMapSize = glm::vec2(4096.0, 4096.0);
	directionalLight0->mShadow->mRadius = 1.0f;
	directionalLight0->mShadow->mCamera = OrthographicCamera::create(-projectSize, projectSize, -projectSize, projectSize, -20, 20);

	directionalLight1 = DirectionalLight::create();
	directionalLight1->setPosition(4.0f, 4.0f, -4.0f);
	directionalLight1->mIntensity = 0.8;
	directionalLight1->mColor = glm::vec3(1.0f, 1.0f, 1.0f);
	directionalLight1->lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	directionalLight1->mCastShadow = true;
	directionalLight1->mShadow->mMapSize = glm::vec2(4096.0, 4096.0);
	directionalLight1->mShadow->mBias = -0.03;
	directionalLight1->mShadow->mCamera = OrthographicCamera::create(-projectSize, projectSize, -projectSize, projectSize, -20, 20);

	auto ambientLight = AmbientLight::create();
	ambientLight->mIntensity = 0.2f;

	scene->addChild(directionalLight0);
	//scene->addChild(directionalLight1);
	scene->addChild(ambientLight);

	dragon = AssimpLoader::load("assets/models/dinosaur/source/Rampaging T-Rex.glb");
	dragon->mObject->setScale(0.4, 0.4, 0.4);
	dragon->mObject->getChildren()[1]->rotateY(90);
	dragon->mObject->getChildren()[1]->setPosition(0.0, -1.5, 21.1);

	dragon->mActions[1]->play();
	dragon->mActions[1]->mSpeed = 15.0f;
	scene->addChild(dragon->mObject);

	auto room = AssimpLoader::load("assets/models/poCity/scene.gltf");
	room->mObject->setScale(0.2, 0.2, 0.2);
	room->mObject->setPosition(0.0, 0.0, 0.0);
	scene->addChild(room->mObject);


	return scene;
}

int main() {
	try {
		//DebugLog::getInstance()->mEnableDebug = true;
		scene = makeScene();

		Renderer::Descriptor rDc;
		rDc.mWidth = 1200;
		rDc.mHeight = 800;

		camera = PerspectiveCamera::create(0.1, 10000, static_cast<float>(rDc.mWidth) / static_cast<float>(rDc.mHeight), 45.0f);
		camera->setPosition(0, 4, 20);

		cameraControl = GameCameraControl::create(camera);
		cameraControl->setSpeed(0.05f);

		renderTarget = RenderTarget::create(rDc.mWidth, rDc.mHeight, RenderTarget::Options());

		Renderer::Ptr renderer = Renderer::create(rDc);
		renderer->setMouseActionCallback(onMouseAction);
		renderer->setKeyboardActionCallBack(onKeyboardAction);
		renderer->setFrameSizeCallBack(onResize);
		renderer->setMouseMoveCallBack(onMouseMove);

		float lastTime = glfwGetTime();
		while (true) {
	
			float currentTime = glfwGetTime();
			float deltaTime = currentTime - lastTime;
			lastTime = currentTime;

			dragon->mActions[1]->update(deltaTime);
			cameraControl->update();
			if (!renderer->render(scene, camera)) {
				break;
			}

			renderer->swap();
			rotateLight();
		}
	}
	catch (std::exception e) {
		std::cout << e.what() << std::endl;
	}


	return 0;
}