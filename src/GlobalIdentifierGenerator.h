#pragma once

#include <cstdint>

class GlobalIdentifierGenerator {
public:
	static int64_t last_id;

public:
	static int64_t generate_id();
};