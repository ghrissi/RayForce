#include "scene.h"

Scene::Scene() {
	physicsScene = Window::physicsManager->CreateScene();

	PxMaterial* defaultMaterial = Window::physicsManager->GetMaterial(MaterialID::Basic);

	PxRigidStatic* groundPlane = PxCreatePlane(*Window::physicsManager->Physics, PxPlane(0, 1, 0, 0), *defaultMaterial);
	if (!groundPlane) {
		std::cerr << "Failed to create ground plane." << std::endl;
		return;
	}
	physicsScene->addActor(*groundPlane);
}

Scene::~Scene() {
	for (Entity* entity : entities) {
		delete entity;
	}
	entities.clear();

	physicsScene->release();
}

void Scene::AddEntity(Entity* entity) {
	entities.push_back(entity);

	entity->Init();

	if (entity->hitbox != nullptr) {
		// Solo lo añadimos si la pose es válida para evitar el spam
		if (entity->hitbox->getGlobalPose().isValid()) {
			physicsScene->addActor(*entity->hitbox);
		}
	}

	entity->Sync();
}

void Scene::RemoveEntity(Entity* entity) {
	auto it = std::find(entities.begin(), entities.end(), entity);
	if (it != entities.end()) {
		entities.erase(it);
	}
}

float fixedStep = 1.0f / 60.0f;
float accumulator = 0.0f;

void Scene::Render() {
	float frameTime = GetFrameTime();

	if (Window::isPaused || frameTime > 0.25 || frameTime < 0) {
		frameTime = fixedStep;
	}

	physicsScene->simulate(frameTime);

	DrawGrid(1000, 1.0f);

	for (Entity* entity : entities) {
		entity->Render();
	}

	Window::renderManager->RenderBuffer();
}

void Scene::Update() {
	if (IsKeyPressed(KEY_F11)) {
		if (!IsWindowFullscreen()) { 
			int monitor = GetCurrentMonitor();
			SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
			ToggleFullscreen(); 
		} else {
			ToggleFullscreen();
			if (window != nullptr) SetWindowSize(window->width, window->height);
		}
	}

	physicsScene->fetchResults(true);

	Tick();

	//Window::physicsManager->UpdateEntities(physicsScene);

	for (Entity* entity : entities) {
		if (entity == nullptr) {
			RemoveEntity(entity);
			continue;
		}

		/*
		if (!fuck) {
			entity->Update();
			continue;
		}*/

		entity->PhysicsUpdate();

		entity->Update();
	}

	if (nextScene != nullptr) {
		Window::sceneManager->LoadScene(nextScene);
	}
}

PxScene* Scene::GetPhysicsScene() {
	return physicsScene;
}


void Scene::LoadScene(Scene* scene) {
	nextScene = scene;
}