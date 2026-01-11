#include "render.h"

RenderManager::RenderManager() {
    
}

RenderManager::~RenderManager() {
    ClearRenderBuffer();
    renderBuffer.clear();
}

void RenderManager::AddModelToRenderBuffer(Model* model, Matrix t) {
    if (model == nullptr) {
        TraceLog(LOG_WARNING, "model was null in RenderManager::AddModelToRenderBuffer");
        return;
    }

    renderBuffer[model].emplace_back(t);
}

void RenderManager::ClearRenderBuffer() {
    for (auto it = renderBuffer.begin(); it != renderBuffer.end(); ++it) {
        auto& matrices = it->second;

        if (matrices.empty()) { 
            continue;
        }

        matrices.clear();
    }
}

void RenderManager::RenderBuffer() {
    for (auto it = renderBuffer.begin(); it != renderBuffer.end(); ++it) {
        auto& model = it->first;
        auto& matrices = it->second;

        if (matrices.empty()) continue;

        int instanceCount = (int)matrices.size();
        Matrix* dataPtr = matrices.data();

        for (int i = 0; i < model->meshCount; i++) {
            int materialIndex = model->meshMaterial[i];

            DrawMeshInstanced(
                model->meshes[i],
                model->materials[materialIndex],
                dataPtr,
                instanceCount
            );
        }

        /*
        for (auto& e : matrices) {
            Matrix old = model->transform;
            model->transform = e;
            DrawModel(*model, { 0, 0, 0 }, 1, BLUE);
            model->transform = old;
        }
        */

        matrices.clear();
    }
}