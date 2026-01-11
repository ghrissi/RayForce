#pragma once

#include "../globals.h"
#include "physics.h"

enum class ModelID : uint16_t {
	#define MODEL_DEF(id, filepath, material) id,
	#include "models.list"
	#undef MODEL_DEF
};

class ModelManager {
	std::unordered_map<ModelID, Model> models;
	const std::map<ModelID, std::string> modelFilepaths = {
		#define MODEL_DEF(id, filepath, material) { ModelID::id, filepath },
		#include "models.list"
		#undef MODEL_DEF
	};

	const std::map<ModelID, MaterialID> modelMaterials = {
		#define MODEL_DEF(id, filepath, material) { ModelID::id, MaterialID::material },
		#include "models.list"
		#undef MODEL_DEF
	};

	Shader shdr;
public:
	ModelManager();
	~ModelManager();

	Model& GetModel(ModelID id);
	void UnloadModel(ModelID id);

	PxMaterial* GetModelMaterial(ModelID id);
};

