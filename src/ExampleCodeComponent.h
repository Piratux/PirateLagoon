#pragma once

#include "CodeComponent.h"
#include "ExampleDataComponent.h"

class ExampleCodeComponent final : public CodeComponent {
private:
	ExampleDataComponent& data_component;

public:
	ExampleCodeComponent(ExampleDataComponent& data_component);

	bool update(float delta) override;
	void draw() override;

private:
};