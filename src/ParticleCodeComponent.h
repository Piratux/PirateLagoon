#pragma once

#include "CodeComponent.h"
#include "ParticleDataComponent.h"

class ParticleCodeComponent final : public CodeComponent {
private:
	ParticleDataComponent& data_component;

public:
	ParticleCodeComponent(ParticleDataComponent& data_component);

	bool update(float delta) override;
	void draw() override;

private:
};