#pragma once
#include "AssetManager.h"

class GlobalAssetManager {
public:
	static AssetManager* asset_manager;

public:
	static void set(AssetManager* asset_manager);
	static AssetManager* get();
};