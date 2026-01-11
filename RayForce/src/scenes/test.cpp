#include "test.h"

#include "../entities/testEntity.h"
#include "scene.h"

ModelID model = ModelID::Cube;
int gridSize = 1;
void Test::Init() {
	DisableCursor();

	TraceLog(LOG_INFO, "Initializing Test Scene");
	TraceLog(LOG_INFO, "Creating %d entities", gridSize * gridSize * gridSize);

	for (int i = 0; i < gridSize; i++) {
		for (int j = 0; j < gridSize; j++) {
			for (int k = 0; k < gridSize; k++) {
				Window::sceneManager->GetCurrentScene()->AddEntity(new TestEntity({ ((float)i - gridSize / 2) * 3, (float)j * 3 + 10.0f, ((float)k - gridSize / 2) * 3 }, model));
			}
		}
	}

	TraceLog(LOG_INFO, "Finished creating entities.");
}

bool toggleMouse = false;
void Test::Tick() {
	if (IsKeyPressed(KEY_M)) {
		if (model == ModelID::Cube) model = ModelID::DamagedHelmet;
		else model = ModelID::Cube;
	}

	if (IsKeyPressed(KEY_R)) {
		LoadScene(new Test());
		TraceLog(LOG_INFO, "Reloading Test Scene");

		if (IsKeyDown(KEY_LEFT_SHIFT)) gridSize -= (gridSize - 1 == 0) ? 0 : 1;
        else gridSize++;
	}

	if (IsKeyPressed(KEY_TAB)) {
		if (toggleMouse) {
			DisableCursor();
			toggleMouse = false;
		}
		else {
			EnableCursor();
			toggleMouse = true;
		}
	}
}