#include "entity.h"
#include "../window.h"
#include "../managers/model.h"
#include "../managers/physics.h"
#include "../managers/render.h"

Entity::Entity(Vector3 pos, ModelID _modelID) : position(pos), modelID(_modelID) {
	model = &Window::modelManager->GetModel(modelID);
}

Entity::~Entity() {
	hitbox->release();
	hitbox = nullptr;
}


/*
void Entity::PhysicsUpdate() {
	if (hitbox) {
		// Obtenemos la pose de PhysX
		PxTransform transform = hitbox->getGlobalPose();

		// Sincronizamos posición básica para tu lógica de C++
		position = { transform.p.x, transform.p.y, transform.p.z };
		rotationQuat = { transform.q.x, transform.q.y, transform.q.z, transform.q.w };

		// Quaterion a Euler
		Vector3 eulerRadianes = QuaternionToEuler(rotationQuat);

		// Convertimos a grados para que tu variable 'rotation' sea fácil de usar
		rotation.x = eulerRadianes.x * RAD2DEG;
		rotation.y = eulerRadianes.y * RAD2DEG;
		rotation.z = eulerRadianes.z * RAD2DEG;

		// Sincronizamos velocidad (solo si la necesitas para lógica)
		PxVec3 v = hitbox->getLinearVelocity();
		velocity = { v.x, v.y, v.z };
	}
}
*/


void Entity::PhysicsUpdate() {
	if (hitbox != nullptr) {
		PxTransform transform = hitbox->getGlobalPose();

		// 1. Guardar posición y cuaternión
		position = { transform.p.x, transform.p.y, transform.p.z };
		rotationQuat = { transform.q.x, transform.q.y, transform.q.z, transform.q.w };

		// 2. OBTENER ÁNGULOS EULER REALES
		// QuaternionToEuler devuelve radianes, multiplicamos por RAD2DEG para grados
		Vector3 eulerRadianes = QuaternionToEuler(rotationQuat);
		rotation.x = eulerRadianes.x * RAD2DEG;
		rotation.y = eulerRadianes.y * RAD2DEG;
		rotation.z = eulerRadianes.z * RAD2DEG;

		// 3. Mantener la matriz para el renderizado eficiente
		PxMat44 physxMat(transform);

		//if (GetRandomValue(0, 10000) == 1) std::cout << "Pos: " << physxMat.column3.x << ", " << physxMat.column3.y << ", " << physxMat.column3.z << "\n";

		// 4. Convertir a Matrix de Raylib
		//memcpy(&worldMatrix, &physxMat, sizeof(Matrix));
		
		// --- ROTACIÓN Y ESCALA (Bloque 3x3) ---
		worldMatrix.m0 = physxMat.column0.x;
		worldMatrix.m1 = physxMat.column0.y;
		worldMatrix.m2 = physxMat.column0.z;
		worldMatrix.m3 = physxMat.column0.w;

		worldMatrix.m4 = physxMat.column1.x;
		worldMatrix.m5 = physxMat.column1.y;
		worldMatrix.m6 = physxMat.column1.z;
		worldMatrix.m7 = physxMat.column1.w;

		worldMatrix.m8 = physxMat.column2.x;
		worldMatrix.m9 = physxMat.column2.y;
		worldMatrix.m10 = physxMat.column2.z;
		worldMatrix.m11 = physxMat.column2.w;

		// Posicíon
		worldMatrix.m12 = physxMat.column3.x;
		worldMatrix.m13 = physxMat.column3.y;
		worldMatrix.m14 = physxMat.column3.z;
		worldMatrix.m15 = 1.0f; // Factor homogéneo (siempre 1)

		// Velocidad
		PxVec3 v = hitbox->getLinearVelocity();
		velocity = {v.x, v.y, v.z};
	}
}


void Entity::Sync() {
	if (hitbox != nullptr) {
		PxQuat q = { rotationQuat.x, rotationQuat.y, rotationQuat.z, rotationQuat.w };
		hitbox->setGlobalPose(PxTransform(PxVec3(position.x, position.y, position.z), {q}));
		//hitbox->setGlobalPose(PxTransform(PxVec3(position.x, position.y, position.z), PxQuat(PxIdentity)));
		hitbox->setLinearVelocity(PxVec3(velocity.x, velocity.y, velocity.z));
	}
}


void Entity::Render() {
	if (model) {
		/*// Guardamos la matriz original del modelo
		Matrix old = model->transform;

		// El orden de multiplicación en Raylib es: Escala * Rotación/Posición
		//Matrix t = MatrixMultiply(scaleMat, worldMatrix);
		Matrix t = worldMatrix;
		model->transform = t;

		if (GetRandomValue(0, 10000) == 1) std::cout << "Pos: " << t.m12 << ", " << t.m13 << ", " << t.m14 << "\n";


		// Dibujamos en 0,0,0 porque la matriz ya dice dónde está el objeto en el mundo
		DrawModel(*model, { 0, 0, 0 }, 1.0f, RED);

		// Restauramos la matriz original del modelo
		model->transform = old;

		*/
		Window::renderManager->AddModelToRenderBuffer(model, worldMatrix);
	}

	/*
	if (hitbox != nullptr) {
		Vector3	axis;
		float angle;

		QuaternionToAxisAngle(rotationQuat, &axis, &angle);

		DrawModelWiresEx(*model, position, axis, angle, scale, RED);
	}
	*/
}

/*
void Entity::Render() {
	if (model) {
		Vector3 axis = { rotationQuat.x, rotationQuat.y, rotationQuat.z };
		float angle = rotationQuat.w * RAD2DEG;

		DrawModelEx(*model, position, axis, angle, scale, WHITE);
	}
*/

void Entity::SetHitbox(PxGeometry* pgeometry) {
	if (pgeometry == nullptr) { 
		TraceLog(LOG_WARNING, "Entity::SetHitbox called with null geometry.");
		return; 
	}

	PxGeometry& geometry = *pgeometry;
	// 1. Asegurar valores iniciales VÁLIDOS en la clase Entity
	if (!std::isfinite(position.x)) position = { 0, 0, 0 };
	if (mass <= 0.0f) mass = 1.0f;

	// 2. Crear el actor con un Transform limpio (Identidad)
	PxTransform safeTransform(PxVec3(position.x, position.y, position.z));

	// Forzamos que el cuaternión sea 0,0,0,1 (Identidad) para evitar el #DEN
	safeTransform.q = PxQuat(PxIdentity);

	if (hitbox == nullptr) {
		hitbox = Window::physicsManager->Physics->createRigidDynamic(safeTransform);
		hitbox->userData = (void*)this;
	}

	// 3. Crear Shape y Material
	PxMaterial* material = Window::modelManager->GetModelMaterial(modelID);
	
	PxShape* shape = PxRigidActorExt::createExclusiveShape(*hitbox, geometry, *material);

	shape->setContactOffset(0.02f); // Un margen de 2cm para cálculos de colisión
	shape->setRestOffset(0.0f);     // Distancia de reposo

	// 4. EL TRUCO: Primero calcula la masa y LUEGO limpia las velocidades
	PxRigidBodyExt::setMassAndUpdateInertia(*hitbox, mass);

	hitbox->setLinearVelocity(PxVec3(0));
	hitbox->setAngularVelocity(PxVec3(0));

	hitbox->setSleepThreshold(0.2f);
}

void Entity::Update() {

}