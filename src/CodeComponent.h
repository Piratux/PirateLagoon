#pragma once

#include <cstdint>

class CodeComponent {
public:
	int64_t entity_id;

public:
	virtual ~CodeComponent() = default;
	virtual bool update(float delta) = 0; // Returns false if component wants to destroy entity that owns it.
	virtual void draw() = 0;
};