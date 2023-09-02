#include "GlobalIdentifierGenerator.h"

#include <iostream>

int64_t GlobalIdentifierGenerator::last_id = 0;

int64_t GlobalIdentifierGenerator::generate_id() {
	// Might detect memory corruption
	if (GlobalIdentifierGenerator::last_id < 0) {
		std::cout << "last_id is negative" << std::endl;
		exit(-1);
	}

	GlobalIdentifierGenerator::last_id++;
	return GlobalIdentifierGenerator::last_id;
}
