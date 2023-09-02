#pragma once

#include "CodeComponent.h"
#include "ProjectileDataComponent.h"

class ProjectileCodeComponent final : public CodeComponent {
private:
	ProjectileDataComponent& data_component;

public:
	ProjectileCodeComponent(ProjectileDataComponent& data_component);

	bool update(float delta) override;
	void draw() override;

	void create_water_splash();

private:
};