#pragma once

#include "../globals.h"

enum class MaterialID : uint16_t {
	#define MATERIAL_DEF(id, ...) id,
	#include "materials.list"
	#undef MATERIAL_DEF
};

class PhysicsManager {
	const std::map<MaterialID, Vector3> materialsProperties = {
		#define MATERIAL_DEF(id, ...) { MaterialID::id, {__VA_ARGS__} },
		#include "materials.list"
		#undef MODEL_DEF
	};

	std::unordered_map<MaterialID, PxMaterial*> loadedMaterials;
	std::unordered_map<uint16_t, PxGeometry*> loadedGeometries;
	std::unordered_map<PxGeometry*, PxShape*> loadedShapes;
public:
	static PxDefaultAllocator Allocator; // Allocator for PhysX
	static PxDefaultErrorCallback ErrorCallback; // Error callback for PhysX

#if !defined(DONT_USE_CUDA)
    physx::PxCudaContextManager* CudaContextManager = nullptr; //Control Cuda
#endif

	PxSceneDesc* SceneDesc = nullptr; // Scene description for PhysX

	PxFoundation* Foundation = nullptr; // Foundation for PhysX
	PxPhysics* Physics = nullptr; // Physics instance for PhysX
	PxDefaultCpuDispatcher* Dispatcher = nullptr; // CPU Dispatcher for PhysX
	PxCooking* Cooking = nullptr; // Cooking interface for PhysX

	PhysicsManager();
	~PhysicsManager();

	PxScene* CreateScene();
	PxScene* CreateScene(PxSceneDesc _SceneDesc);

	PxMaterial* GetMaterial(MaterialID id);
	void UnloadMaterial(MaterialID id);

	PxGeometry* CreateGeometry(uint16_t _modelID);
	void UnloadGeometry(uint16_t _modelID);
	void DeleteGeometry(PxGeometry* geo);

	PxShape* CreateShape(PxGeometry* geometry, PxMaterial* material);
	void UnloadShape(PxGeometry* geometry);

	void UpdateEntities(PxScene* scene);
};

