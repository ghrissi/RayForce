#include "window.h"
#include "scenes/scene.h" // <-- A�adir esta l�nea para incluir la definici�n completa de Scene

SceneManager* Window::sceneManager = nullptr;
ModelManager* Window::modelManager = nullptr;
PhysicsManager* Window::physicsManager = nullptr;
RenderManager* Window::renderManager = nullptr;


bool Window::isPaused = false;

Window* window = nullptr;

#if defined(_WIN32) || defined(_WIN64)
	extern "C" void AttachHook(void* windowHandle, bool* pauseFlag);
#endif

Window::Window(int _width, int _height, const std::string& title, unsigned int flags) {

#if defined(_WIN32) || defined(_WIN64)
	AttachHook(GetWindowHandle(), &isPaused);
#endif
	
	width = _width;
	height = _height;
	
	InitWindow(width, height, title.c_str());

	while(!IsWindowReady());

	SetConfigFlags(flags);

	SetTargetFPS(60);

	camera = { 0 };
	camera.position = { 10.0f, 10.0f, 10.0f }; // Camera position
	camera.target = { 0.0f, 0.0f, 0.0f };      // Looking at the origin
	camera.up = { 0.0f, 1.0f, 0.0f };          // Camera up vector
	camera.fovy = 45.0f;                                // Field of view
	camera.projection = CAMERA_PERSPECTIVE;

	sceneManager = new SceneManager();
	modelManager = new ModelManager();
	physicsManager = new PhysicsManager();
	renderManager = new RenderManager();

	Init();

	sceneManager->GetCurrentScene()->GetPhysicsScene()->simulate(1.f/60.f);


	while (!WindowShouldClose()){
		if (IsCursorHidden()) UpdateCamera(&camera, CAMERA_FREE);
		//std::cout << GetMouseX() << ", " << GetMouseY() << std::endl;
		//std::cout << camera.target.x << ", " << camera.target.y << ", " << camera.target.z << std::endl;

		sceneManager->Update();

		BeginDrawing();

		ClearBackground(BLACK);

		BeginMode3D(camera);

		sceneManager->Render();

		EndMode3D();
		DrawFPS(10, 10);
		EndDrawing();
	}
}

Window::~Window() {
	if (renderManager) delete renderManager;

	if (physicsManager) delete physicsManager;

	if (sceneManager) delete sceneManager;

	if (modelManager) delete modelManager;

	CloseWindow();
}
