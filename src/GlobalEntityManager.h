#pragma once
#include "EntityManager.h"

class GlobalEntityManager {
public:
	static EntityManager* entity_manager;

public:
	static void set(EntityManager* entity_manager);
	static EntityManager* get();
};
