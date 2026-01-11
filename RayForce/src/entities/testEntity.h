#pragma once
#include "entity.h"

class TestEntity : public Entity {
public:
    TestEntity(Vector3 _pos, ModelID _model) : Entity(_pos, _model) {}

    void Init() override;
    void Update() override;
};