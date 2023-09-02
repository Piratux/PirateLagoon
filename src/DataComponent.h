#pragma once

#include <cstdint>

class DataComponent {
public:
	int64_t entity_id;

public:
	virtual ~DataComponent() = default;
};