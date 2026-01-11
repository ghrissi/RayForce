#include "scenes.h"

#include "../scenes/scene.h"

SceneManager::~SceneManager() {
	UnloadScene();
}

void SceneManager::LoadScene(Scene* scene) {
	if (scene == nullptr) {
		return;
	}

	if (currentScene != nullptr) {
		UnloadScene();
	}

	currentScene = scene;
	currentScene->Init();
}

void SceneManager::UnloadScene() {
	if (currentScene == nullptr) {
		return;
	}

	delete currentScene;
	currentScene = nullptr;
}

void SceneManager::Render() {
	if (currentScene == nullptr) {
		return;
	}

	currentScene->Render();
}

void SceneManager::Update() {
	if (currentScene == nullptr) {
		return;
	}

	currentScene->Update();
}

Scene* SceneManager::GetCurrentScene() {
	return currentScene;
}