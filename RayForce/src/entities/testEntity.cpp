#include "testEntity.h"
#include "../managers/physics.h"
#include "../window.h"

void TestEntity::Init() {
	// Código de inicialización para la entidad Cube
	//PxBoxGeometry box(0.5, 0.5, 0.5);

	PxGeometry* box = Window::physicsManager->CreateGeometry((uint16_t)modelID);
	SetHitbox(box); // Establecer el hitbox usando la sobrecarga correcta
	//position = { (float)GetRandomValue(-20, 20), 5.0f + GetRandomValue(0, 20), (float)GetRandomValue(-20, 20)};
	//velocity = { (float)GetRandomValue(-20, 20), (float)GetRandomValue(-20, 20), (float)GetRandomValue(-20, 20)};
	//rotation = { (float)GetRandomValue(0, 360), (float)GetRandomValue(0, 360), (float)GetRandomValue(0, 360) };
}

void TestEntity::Update() {
	// Update logic for Cube entity
}