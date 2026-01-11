#pragma once

#include "../globals.h"

class RenderManager {
	std::unordered_map<Model*, std::vector<Matrix>> renderBuffer;
public:
	RenderManager();
	~RenderManager();

	void AddModelToRenderBuffer(Model* model, Matrix t);
	void ClearRenderBuffer();
	void RenderBuffer();
};

