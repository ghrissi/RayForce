#pragma once

#include "globals.h"

#include "managers/model.h"
#include "managers/scenes.h"
#include "managers/physics.h"
#include "managers/render.h"

class Window {
protected:
	void Render();
	void Update();

	void Init();
public:
	int width, height;
	Camera camera;

	Window(int _width, int _height, const std::string& title, unsigned int flags);
	~Window();

	static SceneManager* sceneManager;
	static ModelManager* modelManager;
	static PhysicsManager* physicsManager;
	static RenderManager* renderManager;
	static bool isPaused;
};

extern Window* window;