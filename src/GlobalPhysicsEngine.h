#pragma once
#include "PhysicsEngine.h"

class GlobalPhysicsEngine {
public:
	static PhysicsEngine* physics_engine;

public:
	static void set(PhysicsEngine* physics_engine);
	static PhysicsEngine* get();
};
