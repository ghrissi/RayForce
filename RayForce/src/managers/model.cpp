#include "model.h"
#include "physics.h"
#include "../window.h"

ModelManager::ModelManager() {
	shdr = LoadShader((SAHDERS_PATH + std::string("instancing.vs")).c_str(), (SAHDERS_PATH + std::string("basic.fs")).c_str());

	if (!IsShaderValid(shdr)) {
		TraceLog(LOG_ERROR, "Fatal Error! The sahders didn`t load!!!");
		return;
	}

	shdr.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(shdr, "mvp");
	shdr.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocationAttrib(shdr, "instanceTransform");
}

ModelManager::~ModelManager() {
	for (auto& pair : models) {
		UnloadModel(pair.first);
	}

	models.clear();

	UnloadShader(shdr);
}

Model& ModelManager::GetModel(ModelID id) {
	auto it = models.find(id);
	if (it != models.end()) {
		return it->second;
	}

	std::string path = std::string(MODELS_PATH + modelFilepaths.at(id));
	Model model;

	if (!FileExists(path.c_str())) {
		TraceLog(LOG_ERROR, "Fail to find file: %s", path.c_str());
		goto error;
	}

	model = ::LoadModel(path.c_str());

	if (model.meshes == nullptr) {
		TraceLog(LOG_ERROR, "Failed to load model: %s", path.c_str());
		goto error;
	}

	goto done;
error: // Cuando hay un error, cargar un modelo por defecto
	model = ::LoadModelFromMesh(GenMeshCube(1, 1, 1));
	model.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = PURPLE;

done: // Despues del errores
	/*
	for (int i = 0; i < model.materialCount; i++) {
		model.materials[i].shader = LoadShader(0, 0);
	}
	*/

	for (int i = 0; i < model.materialCount; i++) {
		model.materials[i].maps[MATERIAL_MAP_DIFFUSE].color = WHITE;
		model.materials[i].shader = shdr;
	}

	models[id] = model;

	return models.at(id);
};

void ModelManager::UnloadModel(ModelID id) {
	if (models.find(id) == models.end()) {
		TraceLog(LOG_WARNING, "Model with id %d no loaded", id);
		return;
	}

	models.erase(id);
};

PxMaterial* ModelManager::GetModelMaterial(ModelID id) {
	MaterialID materialID = modelMaterials.at(id);
	return Window::physicsManager->GetMaterial(materialID);
}