#include "GlobalEntityManager.h"
#include "Utils.h"

EntityManager* GlobalEntityManager::entity_manager = nullptr;

void GlobalEntityManager::set(EntityManager* entity_manager) {
	Utils::check_null(entity_manager);
	GlobalEntityManager::entity_manager = entity_manager;
}

EntityManager* GlobalEntityManager::get() {
	Utils::check_null(GlobalEntityManager::entity_manager);
	return GlobalEntityManager::entity_manager;
}
