#include "EntityManager.h"

#include <stack>

void EntityManager::update(float delta) {
	std::stack<int> entity_deletion_idxs;

	for (size_t i = 0; i < entities.size(); ++i) {
		// if entity needs to be deleted
		if (entities[i]->update(delta) == false) {
			entity_deletion_idxs.push((int)i);
		}
	}

	// entities must be deleted from the last index towards 0 to make sure that deletion indexes are valid
	while (entity_deletion_idxs.empty() == false) {
		std::swap(entities[entity_deletion_idxs.top()], entities.back());
		entities.pop_back();
		entity_deletion_idxs.pop();
	}
}

void EntityManager::draw() {
	for (size_t i = 0; i < entities.size(); ++i) {
		entities[i]->draw();
	}
}

Entity& EntityManager::create_entity() {
	entities.emplace_back(std::make_unique<Entity>());
	return *entities.back().get();
}

Entity* EntityManager::get_entity(int64_t id) {
	for (size_t i = 0; i < entities.size(); ++i) {
		if (entities[i]->get_id() == id) {
			return entities[i].get();
		}
	}

	return nullptr;
}

const std::vector<std::unique_ptr<Entity>>& EntityManager::get_entities() {
	return entities;
}

void EntityManager::destroy_all_entities() {
	entities.clear();
}