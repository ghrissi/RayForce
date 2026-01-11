#pragma once
#include "../globals.h"
#include "../managers/model.h"

// HitBoxes

class Entity {
protected:
	Matrix worldMatrix;

	Quaternion rotationQuat = { 0.0f, 0.0f, 0.0f, 1.0f };;
	Vector3 position = {0, 0, 0};
	Vector3 rotation = {0, 0, 0};
	Vector3 scale = { 1, 1, 1 };
	Vector3 velocity = { 0, 0, 0 };
	Model* model;

	float mass = 10.0f;

	ModelID modelID;
public:
	PxRigidDynamic* hitbox = nullptr;

	Entity(Vector3 pos, ModelID _modelID);
	~Entity();


	void PhysicsUpdate();

	void SetHitbox(PxGeometry* geometry);

	void Sync();

	void Render();
	virtual void Update();
	virtual void Init() {};
};
