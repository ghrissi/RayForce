#include "physics.h"
#include "model.h"
#include "../window.h"

PxDefaultAllocator PhysicsManager::Allocator; // Allocator for PhysX
PxDefaultErrorCallback PhysicsManager::ErrorCallback; // Error callback for PhysX

PhysicsManager::PhysicsManager() {
	// 1. Foundation y Physics primero
	Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, Allocator, ErrorCallback);
	Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *Foundation, PxTolerancesScale(), true);

	// 2. CREAR EL DESC ANTES DE USARLO
	SceneDesc = new PxSceneDesc(Physics->getTolerancesScale());
	SceneDesc->gravity = PxVec3(0.0f, -9.81f, 0.0f);
	SceneDesc->filterShader = PxDefaultSimulationFilterShader;
	SceneDesc->flags |= PxSceneFlag::eENABLE_ACTIVE_ACTORS;
	SceneDesc->flags |= PxSceneFlag::eADAPTIVE_FORCE;

#if !defined(DONT_USE_CUDA)
	// 3. Intento de GPU (Si falla, no pasa nada, SceneDesc ya existe)
	PxCudaContextManagerDesc cudaContextManagerDesc;
	CudaContextManager = PxCreateCudaContextManager(*Foundation, cudaContextManagerDesc, PxGetProfilerCallback());

	if (CudaContextManager && CudaContextManager->contextIsValid()) {
		SceneDesc->cudaContextManager = CudaContextManager;
		SceneDesc->flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;
		SceneDesc->broadPhaseType = PxBroadPhaseType::eGPU;
		TraceLog(LOG_INFO, "Using GPU to calculate physics");
	}
	else {
		TraceLog(LOG_WARNING, "Fail to us GPU acceleration, using CPU");
	}
#endif

	// 4. Dispatcher para CPU (Siempre necesario como respaldo)
	#undef max
	unsigned int optimalThreads = std::max(2u, std::thread::hardware_concurrency() / 2);
	Dispatcher = PxDefaultCpuDispatcherCreate(optimalThreads);
	SceneDesc->cpuDispatcher = Dispatcher;

	// 5. Crear el Cooking
	PxCookingParams params(Physics->getTolerancesScale());

	// Habilitar la limpieza y el soldado de v�rtices
	params.meshPreprocessParams |= PxMeshPreprocessingFlag::eWELD_VERTICES;
	params.meshWeldTolerance = 0.001f;

	params.meshPreprocessParams |= PxMeshPreprocessingFlag::eFORCE_32BIT_INDICES; // Opcional, solo si usas mallas muy grandes

	Cooking = PxCreateCooking(PX_PHYSICS_VERSION, *Foundation, params);
}

PhysicsManager::~PhysicsManager() {
	if (SceneDesc) {
		delete SceneDesc;
		SceneDesc = nullptr;
	}

	for (auto& pair : loadedMaterials) {
		pair.second->release();
	}

	loadedMaterials.clear();

	for (auto& pair : loadedGeometries) {
		DeleteGeometry(pair.second);
	}

	loadedGeometries.clear();

	for (auto& pair : loadedShapes) {
		pair.second->release();
	}

	loadedShapes.clear();

	if (Cooking) {
		Cooking->release();
	}

	if (Dispatcher) {
		Dispatcher->release();
		Dispatcher = nullptr;
	}

#if !defined(DONT_USE_CUDA)
	if (CudaContextManager) {
		CudaContextManager->release();
		CudaContextManager = nullptr;
	}
#endif

	if (Physics) {
		Physics->release();
		Physics = nullptr;
	}

	if (Foundation) {
		Foundation->release();
		Foundation = nullptr;
	}
}

PxScene* PhysicsManager::CreateScene() {
	PxScene* scene = Physics->createScene(*SceneDesc);
	return scene;
}

PxScene* PhysicsManager::CreateScene(PxSceneDesc _SceneDesc) {
	if (!SceneDesc) {
		SceneDesc = &_SceneDesc;
		return CreateScene();
	}

	delete SceneDesc;
	SceneDesc = &_SceneDesc;
	return CreateScene();
}

PxMaterial* PhysicsManager::GetMaterial(MaterialID id) {
	auto it = loadedMaterials.find(id);
	if (it != loadedMaterials.end()) {
		return it->second;
	}

	Vector3 properties = materialsProperties.at(id);

	PxMaterial* material = Physics->createMaterial(
		properties.x,
		properties.y,
		properties.z
	);

	loadedMaterials[id] = material;

	TraceLog(LOG_INFO, "Loaded material ID %d", static_cast<uint16_t>(id));

	return material;
}	

void PhysicsManager::UnloadMaterial(MaterialID id) {
	if (loadedMaterials.find(id) == loadedMaterials.end()) {
		TraceLog(LOG_WARNING, "Material not loaded!");
		return;
	}

	loadedMaterials.at(id)->release();
	loadedMaterials.erase(id);
}

PxGeometry* PhysicsManager::CreateGeometry(uint16_t _modelID) {
	ModelID modelID = (ModelID)_modelID;

	// 1. B�squeda eficiente en cach�
	auto it = loadedGeometries.find(_modelID);
	if (it != loadedGeometries.end()) {
		return it->second;
	}

	Model* model = &Window::modelManager->GetModel(modelID);
	if (!model || model->meshCount == 0) return nullptr;

	// --- NUEVA L�GICA: RECOLECCI�N DE V�RTICES ---
	std::vector<float> allVertices;
	int totalPolygons = 0;

	// Primero calculamos cu�nto espacio necesitamos (evita realocaciones)
	size_t totalVertexCount = 0;
	for (int i = 0; i < model->meshCount; i++) {
		totalVertexCount += model->meshes[i].vertexCount;
		totalPolygons += model->meshes[i].triangleCount;
	}
	allVertices.reserve(totalVertexCount * 3);

	// Copiamos los v�rtices de todas las mallas al vector �nico
	for (int i = 0; i < model->meshCount; i++) {
		Mesh& m = model->meshes[i];
		// Raylib guarda los v�rtices como floats (x, y, z)
		for (int v = 0; v < m.vertexCount * 3; v++) {
			allVertices.push_back(m.vertices[v]);
		}
	}

	TraceLog(LOG_INFO, "Cooking model ID %d (%d mesh, %d poligons)",
		_modelID, model->meshCount, totalPolygons);

	// 1. Configurar descriptor Convexo con el buffer unificado
	PxConvexMeshDesc convexDesc;
	convexDesc.points.count = (PxU32)allVertices.size() / 3;
	convexDesc.points.stride = sizeof(float) * 3;
	convexDesc.points.data = allVertices.data();

	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

	// 2. Cocinar la malla convexa
	PxDefaultMemoryOutputStream writeBuffer;
	PxConvexMeshCookingResult::Enum result;

	if (!Cooking->cookConvexMesh(convexDesc, writeBuffer, &result)) {
		TraceLog(LOG_WARNING, "Failed to create cooking Error : % d", (int)result);
		return nullptr;
	}

	// 3. Crear la malla en el motor
	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	PxConvexMesh* convexMesh = Physics->createConvexMesh(readBuffer);

	if (!convexMesh) return nullptr;

	// 4. Retornar geometr�a convexa
	PxGeometry* geometry = new PxConvexMeshGeometry(convexMesh);
	loadedGeometries[_modelID] = geometry;

	return geometry;
}

void PhysicsManager::UnloadGeometry(uint16_t _modelID) {
	if (loadedGeometries.find(_modelID) == loadedGeometries.end()) {
		TraceLog(LOG_WARNING, "Geometry not loaded!");
		return;
	}

	DeleteGeometry(loadedGeometries[_modelID]);
	loadedGeometries.erase(_modelID);
}

void PhysicsManager::DeleteGeometry(PxGeometry* geo){
	if (!geo) { 
		TraceLog(LOG_WARNING, "Geo was a nullptr in PhysicsManager::DeleteGeometry");
		return; 
	}

	// 1. Liberar el recurso interno (la malla cocinada)
	if (geo->getType() == PxGeometryType::eTRIANGLEMESH) {
		PxTriangleMeshGeometry* triGeo = static_cast<PxTriangleMeshGeometry*>(geo);
		if (triGeo->triangleMesh) {
			triGeo->triangleMesh->release(); // Libera la malla de la memoria de PhysX
		}
	}
	else if (geo->getType() == PxGeometryType::eCONVEXMESH) {
		PxConvexMeshGeometry* convexGeo = static_cast<PxConvexMeshGeometry*>(geo);
		if (convexGeo->convexMesh) {
			convexGeo->convexMesh->release(); // Libera la malla convexa
		}
	}

	// 2. Liberar el contenedor que creamos con 'new'
	delete geo;
	printf("INFO: Geometr�a f�sica liberada correctamente.\n");
}

PxShape* PhysicsManager::CreateShape(PxGeometry* geometry, PxMaterial* material) {
	if (geometry == nullptr) return nullptr;
	if (material == nullptr) return nullptr;


	auto it = loadedShapes.find(geometry);
	if (it != loadedShapes.end()) {
		return it->second;
	}

	PxShape* shape = Physics->createShape(*geometry, *material, true);
	loadedShapes[geometry] = shape;

	return shape;
}


void PhysicsManager::UnloadShape(PxGeometry* geometry) {
	if (loadedShapes.find(geometry) == loadedShapes.end()) {
		TraceLog(LOG_WARNING, "Shape not loaded!");
		return;
	}

	loadedShapes[geometry]->release();
	loadedShapes.erase(geometry);
}

#include "../entities/entity.h"
void PhysicsManager::UpdateEntities(PxScene* scene) {
	PxU32 nbActiveActors;
	PxActor** activeActors = scene->getActiveActors(nbActiveActors);
	//if (nbActiveActors > 200) return false;

	// Solo recorremos los que REALMENTE se movieron (pueden ser 100 de 22,000)
	for (PxU32 i = 0; i < nbActiveActors; ++i) {
		// Usamos el userData que guardaste al crear el hitbox
		Entity* ent = static_cast<Entity*>(activeActors[i]->userData);
		if (ent) {
			ent->PhysicsUpdate();
		}
	}

	//return true;
}