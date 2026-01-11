#pragma once

#include "../globals.h"
#include "../entities/entity.h"
#include "../window.h"

class Scene {
protected:
    PxScene* physicsScene = nullptr;
    PxRigidStatic* groundPlane = nullptr;

    std::vector<Entity*> entities;
    Scene* nextScene = nullptr;
public:

    virtual void Init() {};

    Scene();
    virtual ~Scene();
    virtual void Tick() {};
	void Unload();

    PxScene* GetPhysicsScene();

    void Render();
    void Update();

    void AddEntity(Entity* entity);
    void RemoveEntity(Entity* entity);
    void LoadScene(Scene* scene);
};