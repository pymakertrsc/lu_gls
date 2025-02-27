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
#include "../ff/animation/animationAction.h"
#include "../ff/tools/timer.h"
#include "../ff/log/debugLog.h"
#include "../ff/camera/orthographicCamera.h"

using namespace ff;

//����
Scene::Ptr	scene = nullptr;

//�ذ�ƽ��
Mesh::Ptr	plane = nullptr;

//ƽ�й�
DirectionalLight::Ptr directionalLight = nullptr;

//������
Camera::Ptr camera = nullptr;
GameCameraControl::Ptr cameraControl = nullptr;

//�������
AnimationAction::Ptr action = nullptr;
Timer::Ptr	timer = Timer::create();


static void onMouseMove(double xpos, double ypos) {
	cameraControl->onMouseMove(xpos, ypos);
}

static void onMouseAction(MouseAction mouseAction) {
	if (mouseAction == MouseAction::LeftDown) {
		float speed = action->mSpeed - 0.1;
		action->mSpeed = speed > 0.0 ? speed : 0.001;
	}

	if (mouseAction == MouseAction::RightDown) {
		float speed = action->mSpeed + 0.1;
		action->mSpeed = speed > 0.0 ? speed : 0.001;
	}

	cameraControl->onMouseAction(mouseAction);
}

static void onKeyboardAction(KeyBoardState action) {
	cameraControl->onKeyboard(action);
}

void onResize(int width, int height) {
}

Scene::Ptr makeScene() {
	Scene::Ptr scene = Scene::create();

	//�����ذ�ƽ���Geometry
	auto planeGeometry = PlaneGeometry::create(8.0, 8.0, 1, 1);

	//��Ҫ���ܹ��գ�������Ҫ����ģ�͵���Ӱ
	MeshPhongMaterial::Ptr phongMaterial = MeshPhongMaterial::create();
	phongMaterial->mSide = Side::FrontSide;//ֻ��Ⱦ����

	//���ɵذ�ƽ��
	plane = Mesh::create(planeGeometry, phongMaterial);
	plane->setScale(0.5, 0.5, 0.5);
	plane->rotateX(-90.0f);
	plane->setPosition(0.0, 0.0, 0.0);
	scene->addChild(plane);

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

	directionalLight->mCastShadow = true;
	directionalLight->mShadow->mMapSize = glm::vec2(2048.0, 2048.0);
	directionalLight->mShadow->mBias = -0.003;
	directionalLight->mShadow->mCamera = OrthographicCamera::create(-10, 10, -10, 10, -10, 10);


	auto ambientLight = AmbientLight::create();
	ambientLight->mIntensity = 0.2f;

	scene->addChild(directionalLight);
	scene->addChild(ambientLight);

	auto model = AssimpLoader::load("assets/models/Fist Fight B.fbx");
	///model->mObject->setPosition(0.5, 0.5, 0.5);
	model->mObject->setScale(0.01, 0.01, 0.01);
	scene->addChild(model->mObject);


	//actions���ܻ��ж����ÿһ��������һ�������������ȭ����·���ܲ���
	action = model->mActions[0];
	//	action->mSpeed = 0.3;
	action->play();

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
		cameraControl->setSpeed(0.05f);

		Renderer::Ptr renderer = Renderer::create(rDc);
		renderer->setMouseActionCallback(onMouseAction);
		renderer->setKeyboardActionCallBack(onKeyboardAction);
		renderer->setFrameSizeCallBack(onResize);
		renderer->setMouseMoveCallBack(onMouseMove);

		//����ѭ��֮ǰ�����Ȼ�ȡ��ʼʱ�䣬elapsed_mill���غ����ʱ,
		//����1000.0�� һ���ǽ�int64_tתΪfloat��һ���ǽ�����תΪ���ʱ
		float lastTime = timer->elapsed_mill() / 1000.0f;

		while (true) {
			//1 ��ȡ��ǰ��������ʱ��
			float currentTime = timer->elapsed_mill() / 1000.0f;

			//2 ���㵱ǰʱ�䣬����һ֡ʱ���ʱ���
			float deltaTime = currentTime - lastTime;

			//3 ����ǰʱ�䱣����lastTime���У�׼����һ֡��������
			lastTime = currentTime;

			//����action״̬,���actionû�е���play����������θ���ʱ��
			//�����������н�չ;�����ĳ��ʱ�̣�������action��stop,����Ҳ��ֹͣ
			action->update(deltaTime);

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