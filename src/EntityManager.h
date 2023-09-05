#pragma once

#include "Entity.h"

#include <vector>
#include <memory>

class EntityManager {
private:
	std::vector<std::unique_ptr<Entity>> entities;

public:
	void update(float delta);
	void draw();

	// creates entity and returns reference to it
	Entity& create_entity();

	// returns pointer to entity or nullptr if entity not found
	Entity* get_entity(int64_t id);
	const std::vector<std::unique_ptr<Entity>>& get_entities();
	void destroy_all_entities();
};

