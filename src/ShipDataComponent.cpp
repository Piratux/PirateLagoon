#include "ShipDataComponent.h"
#include "PiraMath.h"
#include "GlobalPhysicsEngine.h"
#include "GlobalAssetManager.h"
#include "EnumColliderFlag.h"

ShipDataComponent::ShipDataComponent(olc::vf2d pos, ShipType ship_type)
	: ship_type(ship_type) {

	PhysicsEngine* physics_engine = GlobalPhysicsEngine::get();
	AssetManager* asset_manager = GlobalAssetManager::get();

	spawn_pos = pos;

	body = physics_engine->create_body(b2BodyType::b2_dynamicBody, pos.x, pos.y);

	ColliderData data;
	data.user_data = reinterpret_cast<uintptr_t>(static_cast<DataComponent*>(this));
	data.mask = ColliderFlag::WALL | ColliderFlag::PLAYER_SHIP | ColliderFlag::ENEMY_SHIP;

	if (ship_type == ShipType::PLAYER) {
		data.category = ColliderFlag::PLAYER_SHIP;
		data.mask |= ColliderFlag::ENEMY_PROJECTILE;
	}
	else {
		data.category = ColliderFlag::ENEMY_SHIP;
		data.mask |= ColliderFlag::PLAYER_PROJECTILE;
	}

	float collider_scale = 1.0f;
	if (ship_type == ShipType::ENEMY_BLACK) {
		collider_scale = black_ship_scale;
		texture_scale = olc::vf2d{black_ship_scale, black_ship_scale};
	}

	physics_engine->add_circle_collider(body, &data, 50.0f * collider_scale);


	switch (ship_type) {
	case ShipType::PLAYER:
		animation_group = asset_manager->get_animation_group(AnimationGroupName::SHIP_WHITE);

		// TEMP
		//acceleration = 10;
		//max_vel = 30;
		//rotation_speed = 200;
		//reload_timer_length_seconds = 0.2;
		//damage = 100;

		break;

	case ShipType::ENEMY_YELLOW: // basic
		acceleration = 0.5f;
		animation_group = asset_manager->get_animation_group(AnimationGroupName::SHIP_YELLOW);
		max_vel = 2.0f;
		damage = 10;
		set_max_health(100);
		reload_timer_length_seconds = 4.0f;
		rotation_speed = 40.0f;

		// TEMP
		//avoid_range = 1000;
		//max_vel = 5.0f;
		//acceleration = 1.0f;
		//set_velocity(max_vel);
		//reload_timer_length_seconds = 0.2;
		//damage = 40;


		break;

	case ShipType::ENEMY_RED: // tankier and more damage but slow
		acceleration = 0.4f;
		animation_group = asset_manager->get_animation_group(AnimationGroupName::SHIP_RED);
		max_vel = 1.5f;
		damage = 15;
		set_max_health(150);
		reload_timer_length_seconds = 5.0f;
		rotation_speed = 35.0f;
		break;

	case ShipType::ENEMY_GREEN: // fast but brittle
		acceleration = 1.0f;
		animation_group = asset_manager->get_animation_group(AnimationGroupName::SHIP_GREEN);
		max_vel = 4.0f;
		damage = 10;
		set_max_health(50);
		reload_timer_length_seconds = 5.0f;
		rotation_speed = 60.0f;
		avoid_range = 400.0f;
		break;

	case ShipType::ENEMY_BLUE: // does special fast attack
		acceleration = 0.8f;
		animation_group = asset_manager->get_animation_group(AnimationGroupName::SHIP_BLUE);
		max_vel = 3.0f;
		damage = 5;
		set_max_health(100);
		reload_timer_length_seconds = 5.0f;
		rotation_speed = 50.0f;
		break;

	case ShipType::ENEMY_BLACK: // boss: bigger, shoots big bullets and very tanky
		acceleration = 0.3f;
		animation_group = asset_manager->get_animation_group(AnimationGroupName::SHIP_BLACK);
		max_vel = 1.0f;
		damage = 40;
		set_max_health(500);
		reload_timer_length_seconds = 6.0f;
		rotation_speed = 20.0f;
		break;

	default:
		break;
	}
}

ShipDataComponent::~ShipDataComponent() {
	PhysicsEngine* physics_engine = GlobalPhysicsEngine::get();

	if (body != nullptr) {
		physics_engine->destroy_body(body);
	}
}

void ShipDataComponent::rotate_ship(ShipSide ship_side, float delta) {
	float rotation_side = (ship_side == ShipSide::LEFT) ? -1.0f : 1.0f;

	float rotation_scale = (vel / max_vel) * (max_rotation_scale - min_rotation_scale) + min_rotation_scale;
	set_rotation_degrees(get_rotation_degrees() + rotation_side * (rotation_speed * delta * rotation_scale));
}

void ShipDataComponent::set_velocity(float new_velocity) {
	vel = new_velocity;
	vel = std::clamp(vel, min_vel, max_vel);
}

float ShipDataComponent::get_velocity() {
	return vel;
}

void ShipDataComponent::set_rotation_degrees(float degrees) {
	PhysicsEngine* physics_engine = GlobalPhysicsEngine::get();

	if (body == nullptr) {
		std::cout << "Body is null in set_rotation_degrees()" << std::endl;
		exit(-1);
	}

	olc::vf2d pos = physics_engine->get_position(body);
	physics_engine->set_transform(body, pos.x, pos.y, PiraMath::degrees_to_radians(degrees));
}

float ShipDataComponent::get_rotation_degrees() {
	PhysicsEngine* physics_engine = GlobalPhysicsEngine::get();

	if (body == nullptr) {
		std::cout << "Body is null in get_rotation_degrees()" << std::endl;
		exit(-1);
	}
	return PiraMath::radians_to_degrees(physics_engine->get_angle_radians(body));
}

void ShipDataComponent::accelerate(float scale, float delta) {
	set_velocity(get_velocity() + scale * acceleration * delta);
}

void ShipDataComponent::reset_regeneration_timer() {
	no_regeneration_timer_seconds = no_regeneration_timer_length_seconds;
}

void ShipDataComponent::reset_reload_timer() {
	reload_timer_seconds = reload_timer_length_seconds;
}

void ShipDataComponent::reset_no_ram_damage_timer() {
	no_ram_damage_timer_seconds = no_ram_damage_timer_length_seconds;
}