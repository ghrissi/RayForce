#pragma once

#include "../globals.h"

class Scene;

class SceneManager {
	Scene* currentScene = nullptr;
public:
	~SceneManager();

	void LoadScene(Scene* scene);
	Scene* GetCurrentScene();
	void UnloadScene();

	void Render();
	void Update();
};

