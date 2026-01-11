#pragma once

#include "scene.h"

class Test : public Scene{
public:
	Test() : Scene() {};
	void Init() override;
	void Tick() override;
};

