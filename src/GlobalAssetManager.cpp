#include "GlobalAssetManager.h"
#include "Utils.h"

AssetManager* GlobalAssetManager::asset_manager = nullptr;

void GlobalAssetManager::set(AssetManager* asset_manager) {
	Utils::check_null(asset_manager);
	GlobalAssetManager::asset_manager = asset_manager;
}

AssetManager* GlobalAssetManager::get() {
	Utils::check_null(GlobalAssetManager::asset_manager);
	return GlobalAssetManager::asset_manager;
}
