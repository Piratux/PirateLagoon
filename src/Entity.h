#pragma once

#include "CodeComponent.h"
#include "DataComponent.h"

#include <memory>
#include <vector>

class Entity final {
private:
	int64_t id = -1;
	std::vector<std::unique_ptr<CodeComponent>> code_components;
	std::vector<std::unique_ptr<DataComponent>> data_components;

public:
	Entity();

	// Return false, if object wants to destroy itself after update.
	// It returns false, if one of the components returns false;
	bool update(float delta);

	void draw();

	int64_t get_id();

	template <typename T, typename... Args>
	T& add_code_component(Args&&... args) {
		std::unique_ptr<T> code_component = std::make_unique<T>(std::forward<Args>(args)...);
		code_component->entity_id = id;
		code_components.emplace_back(std::move(code_component));
		return dynamic_cast<T&>(*code_components.back().get());
	}

	template <typename T, typename... Args>
	T& add_data_component(Args&&... args) {
		std::unique_ptr<T> data_component = std::make_unique<T>(std::forward<Args>(args)...);
		data_component->entity_id = id;
		data_components.emplace_back(std::move(data_component));
		return dynamic_cast<T&>(*data_components.back().get());
	}
};