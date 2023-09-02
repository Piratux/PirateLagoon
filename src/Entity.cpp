#include "Entity.h"
#include "GlobalIdentifierGenerator.h"

Entity::Entity() {
	this->id = GlobalIdentifierGenerator::generate_id();
}

bool Entity::update(float delta) {
	bool return_value = true;

	for (size_t i = 0; i < code_components.size(); ++i) {
		// splitted into several lines to avoid early expression termination
		bool update_value = code_components[i]->update(delta);
		return_value &= update_value;
	}

	return return_value;
}

void Entity::draw() {
	for (size_t i = 0; i < code_components.size(); ++i) {
		code_components[i]->draw();
	}
}

int64_t Entity::get_id() {
	return id;
}