#include "GlobalPhysicsEngine.h"
#include "Utils.h"

PhysicsEngine* GlobalPhysicsEngine::physics_engine = nullptr;

void GlobalPhysicsEngine::set(PhysicsEngine* physics_engine) {
	Utils::check_null(physics_engine);
	GlobalPhysicsEngine::physics_engine = physics_engine;
}

PhysicsEngine* GlobalPhysicsEngine::get() {
	Utils::check_null(GlobalPhysicsEngine::physics_engine);
	return GlobalPhysicsEngine::physics_engine;
}
