#include "ContactListener.h"

#include "EnumColliderFlag.h"
#include "DataComponent.h"
#include "ProjectileDataComponent.h"
#include "ShipDataComponent.h"
#include "Utils.h"
#include "GlobalCamera2D.h"
#include "GlobalEntityManager.h"
#include "GlobalAssetManager.h"
#include "GlobalGameState.h"
#include "ParticleCodeComponent.h"
#include "ParticleDataComponent.h"

#include <cstdint>


ContactListener::ContactListener(PhysicsEngine* physics_engine) {
	physics_engine->set_contact_listener(this);
}

ContactListener::~ContactListener() {
};

void ContactListener::BeginContact(b2Contact* contact) {
	ColliderFlag collider_flag_a = (ColliderFlag)contact->GetFixtureA()->GetFilterData().categoryBits;
	ColliderFlag collider_flag_b = (ColliderFlag)contact->GetFixtureB()->GetFilterData().categoryBits;
	uintptr_t collider_data_a = contact->GetFixtureA()->GetUserData().pointer;
	uintptr_t collider_data_b = contact->GetFixtureB()->GetUserData().pointer;

	if (collider_flag_a & ColliderFlag::PLAYER_SHIP) {
		if (collider_flag_b & ColliderFlag::ENEMY_SHIP) {
			player_ship_vs_enemy_ship_begin(collider_data_a, collider_data_b);
		}
		else if (collider_flag_b & ColliderFlag::ENEMY_PROJECTILE) {
			player_ship_vs_enemy_projectile_begin(collider_data_a, collider_data_b);
		}
		else if (collider_flag_b & ColliderFlag::WALL) {
			player_ship_vs_wall_begin(collider_data_a, collider_data_b);
		}
		else if (collider_flag_b & ColliderFlag::PLAYER_PROJECTILE) {

		}
	}
	else if (collider_flag_a & ColliderFlag::ENEMY_SHIP) {
		if (collider_flag_b & ColliderFlag::PLAYER_SHIP) {
			player_ship_vs_enemy_ship_begin(collider_data_b, collider_data_a);
		}
		else if (collider_flag_b & ColliderFlag::PLAYER_PROJECTILE) {
			enemy_ship_vs_player_projectile_begin(collider_data_a, collider_data_b);
		}
		else if (collider_flag_b & ColliderFlag::WALL) {
			enemy_ship_vs_wall_begin(collider_data_a, collider_data_b);
		}
		else if (collider_flag_b & ColliderFlag::ENEMY_PROJECTILE) {

		}
	}
	else if (collider_flag_a & ColliderFlag::PLAYER_PROJECTILE) {
		if (collider_flag_b & ColliderFlag::ENEMY_SHIP) {
			enemy_ship_vs_player_projectile_begin(collider_data_b, collider_data_a);
		}
		else if (collider_flag_b & ColliderFlag::WALL) {
			wall_vs_player_projectile_begin(collider_data_b, collider_data_a);
		}
	}
	else if (collider_flag_a & ColliderFlag::ENEMY_PROJECTILE) {
		if (collider_flag_b & ColliderFlag::PLAYER_SHIP) {
			player_ship_vs_enemy_projectile_begin(collider_data_b, collider_data_a);
		}
		else if (collider_flag_b & ColliderFlag::WALL) {
			wall_vs_player_projectile_begin(collider_data_b, collider_data_a);
		}
	}
	else if (collider_flag_a & ColliderFlag::WALL) {
		if (collider_flag_b & ColliderFlag::PLAYER_PROJECTILE) {
			wall_vs_player_projectile_begin(collider_data_a, collider_data_b);
		}
		else if (collider_flag_b & ColliderFlag::ENEMY_PROJECTILE) {
			wall_vs_enemy_projectile_begin(collider_data_a, collider_data_b);
		}
		else if (collider_flag_b & ColliderFlag::PLAYER_SHIP) {
			player_ship_vs_wall_begin(collider_data_b, collider_data_a);
		}
		else if (collider_flag_b & ColliderFlag::ENEMY_SHIP) {
			enemy_ship_vs_wall_begin(collider_data_b, collider_data_a);
		}
	}
}
void ContactListener::EndContact(b2Contact* contact) {
	ColliderFlag collider_flag_a = (ColliderFlag)contact->GetFixtureA()->GetFilterData().categoryBits;
	ColliderFlag collider_flag_b = (ColliderFlag)contact->GetFixtureB()->GetFilterData().categoryBits;
	uintptr_t collider_data_a = contact->GetFixtureA()->GetUserData().pointer;
	uintptr_t collider_data_b = contact->GetFixtureB()->GetUserData().pointer;

	if (collider_flag_a & ColliderFlag::PLAYER_SHIP) {
		if (collider_flag_b & ColliderFlag::ENEMY_SHIP) {
			player_ship_vs_enemy_ship_end(collider_data_a, collider_data_b);
		}
		else if (collider_flag_b & ColliderFlag::ENEMY_PROJECTILE) {
			player_ship_vs_enemy_projectile_end(collider_data_a, collider_data_b);
		}
		else if (collider_flag_b & ColliderFlag::WALL) {
			player_ship_vs_wall_end(collider_data_a, collider_data_b);
		}
		else if (collider_flag_b & ColliderFlag::PLAYER_PROJECTILE) {

		}
	}
	else if (collider_flag_a & ColliderFlag::ENEMY_SHIP) {
		if (collider_flag_b & ColliderFlag::PLAYER_SHIP) {
			player_ship_vs_enemy_ship_end(collider_data_b, collider_data_a);
		}
		else if (collider_flag_b & ColliderFlag::PLAYER_PROJECTILE) {
			enemy_ship_vs_player_projectile_end(collider_data_a, collider_data_b);
		}
		else if (collider_flag_b & ColliderFlag::WALL) {
			enemy_ship_vs_wall_end(collider_data_a, collider_data_b);
		}
		else if (collider_flag_b & ColliderFlag::ENEMY_PROJECTILE) {

		}
	}
	else if (collider_flag_a & ColliderFlag::PLAYER_PROJECTILE) {
		if (collider_flag_b & ColliderFlag::ENEMY_SHIP) {
			enemy_ship_vs_player_projectile_end(collider_data_b, collider_data_a);
		}
		else if (collider_flag_b & ColliderFlag::WALL) {
			wall_vs_player_projectile_end(collider_data_b, collider_data_a);
		}
	}
	else if (collider_flag_a & ColliderFlag::ENEMY_PROJECTILE) {
		if (collider_flag_b & ColliderFlag::PLAYER_SHIP) {
			player_ship_vs_enemy_projectile_end(collider_data_b, collider_data_a);
		}
		else if (collider_flag_b & ColliderFlag::WALL) {
			wall_vs_player_projectile_end(collider_data_b, collider_data_a);
		}
	}
	else if (collider_flag_a & ColliderFlag::WALL) {
		if (collider_flag_b & ColliderFlag::PLAYER_PROJECTILE) {
			wall_vs_player_projectile_end(collider_data_a, collider_data_b);
		}
		else if (collider_flag_b & ColliderFlag::ENEMY_PROJECTILE) {
			wall_vs_enemy_projectile_end(collider_data_a, collider_data_b);
		}
		else if (collider_flag_b & ColliderFlag::PLAYER_SHIP) {
			player_ship_vs_wall_end(collider_data_b, collider_data_a);
		}
		else if (collider_flag_b & ColliderFlag::ENEMY_SHIP) {
			enemy_ship_vs_wall_end(collider_data_b, collider_data_a);
		}
	}
}

void ContactListener::wall_vs_player_projectile_begin(uintptr_t wall, uintptr_t player_projectile) {
	ProjectileDataComponent* projectile = reinterpret_cast<ProjectileDataComponent*>(player_projectile);
	projectile->destroy_on_next_frame = true;
}

void ContactListener::wall_vs_enemy_projectile_begin(uintptr_t wall, uintptr_t enemy_projectile) {
	ProjectileDataComponent* projectile = reinterpret_cast<ProjectileDataComponent*>(enemy_projectile);
	projectile->destroy_on_next_frame = true;
}

void ContactListener::player_ship_vs_enemy_ship_begin(uintptr_t player_ship, uintptr_t enemy_ship) {
	Camera2D* camera = GlobalCamera2D::get();
	GameState* game_state = GlobalGameState::get();

	ShipDataComponent* player = reinterpret_cast<ShipDataComponent*>(player_ship);
	ShipDataComponent* enemy = reinterpret_cast<ShipDataComponent*>(enemy_ship);

	if (game_state->get() != GameState::State::ACTION) {
		return;
	}

	float player_speed_ratio = player->vel / player->max_vel;
	float enemy_speed_ratio = enemy->vel / enemy->max_vel;

	// use higher speed ratio for damage
	float speed_ratio = std::max(player_speed_ratio, enemy_speed_ratio);
	if (speed_ratio > 0.5f) {
		// TODO: probably could be improved to take more damage from stronger enemies
		// magical 5 is to make sure damage is done in range [0; 5]
		int damage = (int)std::round(player->ram_wall_damage_percentage * 0.01f * (float)player->max_health * speed_ratio) - 5;

		if (player->take_ram_damage(damage)) {
			add_floating_text_particle(player->pos, "-" + std::to_string(damage));
			camera->small_shake(olc::vf2d{});
		}

		if (enemy->take_ram_damage(damage)) {
			add_floating_text_particle(enemy->pos, "-" + std::to_string(damage));
			camera->small_shake(olc::vf2d{});
		}
	}
}

void ContactListener::player_ship_vs_enemy_projectile_begin(uintptr_t player_ship, uintptr_t enemy_projectile) {
	Camera2D* camera = GlobalCamera2D::get();
	GameState* game_state = GlobalGameState::get();

	ShipDataComponent* player = reinterpret_cast<ShipDataComponent*>(player_ship);
	ProjectileDataComponent* projectile = reinterpret_cast<ProjectileDataComponent*>(enemy_projectile);

	projectile->destroy_on_next_frame = true;

	if (game_state->get() != GameState::State::ACTION) {
		return;
	}

	if (player->take_damage(projectile->damage)) {
		add_floating_text_particle(player->pos, "-" + std::to_string(projectile->damage));
	}

	olc::vf2d projectile_dir = Utils::b2Vec2_to_vf2d(projectile->body->GetLinearVelocity()).norm();
	if (player->is_dead()) {
		camera->big_shake(projectile_dir);
	}
	else {
		camera->small_shake(projectile_dir);
	}
}

void ContactListener::enemy_ship_vs_player_projectile_begin(uintptr_t enemy_ship, uintptr_t player_projectile) {
	Camera2D* camera = GlobalCamera2D::get();
	GameState* game_state = GlobalGameState::get();

	ShipDataComponent* enemy = reinterpret_cast<ShipDataComponent*>(enemy_ship);
	ProjectileDataComponent* projectile = reinterpret_cast<ProjectileDataComponent*>(player_projectile);

	projectile->destroy_on_next_frame = true;

	if (game_state->get() != GameState::State::ACTION) {
		return;
	}

	if (enemy->take_damage(projectile->damage)) {
		add_floating_text_particle(enemy->pos, "-" + std::to_string(projectile->damage));
	}

	// TODO: would be a great idea to put this formula in ExplosionParticle static method
	//float explosion_scale = 0.2f + temp_projectile.texture_scale.x * 0.2f;

	olc::vf2d projectile_dir = Utils::b2Vec2_to_vf2d(projectile->body->GetLinearVelocity()).norm();
	if (enemy->is_dead()) {
		camera->big_shake(projectile_dir);
	}
	else {
		camera->small_shake(projectile_dir);
	}
}

void ContactListener::player_ship_vs_wall_begin(uintptr_t player_ship, uintptr_t wall) {
	Camera2D* camera = GlobalCamera2D::get();
	GameState* game_state = GlobalGameState::get();

	ShipDataComponent* player = reinterpret_cast<ShipDataComponent*>(player_ship);
	player->is_colliding = true;

	if (game_state->get() != GameState::State::ACTION) {
		return;
	}

	float speed_ratio = player->vel / player->max_vel;
	if (speed_ratio > 0.5f) {
		int damage = (int)std::round(player->ram_wall_damage_percentage * 0.01f * (float)player->max_health * speed_ratio) - 5; // magical 5 is to make sure damage is done in range [0; 5]
		if (player->take_ram_damage(damage)) {
			add_floating_text_particle(player->pos, "-" + std::to_string(damage));
			camera->small_shake(olc::vf2d{});
		}
	}
}

void ContactListener::enemy_ship_vs_wall_begin(uintptr_t enemy_ship, uintptr_t wall) {
	ShipDataComponent* enemy = reinterpret_cast<ShipDataComponent*>(enemy_ship);
	// TODO: This should be put somewhere else, because taking ram damage also has this line
	enemy->set_velocity(enemy->get_velocity() - enemy->ram_impact_lost_speed_percentage * 0.01f * enemy->max_vel);
}

void ContactListener::player_projectile_vs_enemy_projectile_begin(uintptr_t player_projectile, uintptr_t enemy_projectile) {
}


void ContactListener::wall_vs_player_projectile_end(uintptr_t wall, uintptr_t player_projectile) {

}

void ContactListener::wall_vs_enemy_projectile_end(uintptr_t wall, uintptr_t enemy_projectile) {

}

void ContactListener::player_ship_vs_enemy_ship_end(uintptr_t player_ship, uintptr_t enemy_ship) {

}

void ContactListener::player_ship_vs_enemy_projectile_end(uintptr_t player_ship, uintptr_t enemy_projectile) {

}

void ContactListener::enemy_ship_vs_player_projectile_end(uintptr_t enemy_ship, uintptr_t player_projectile) {

}

void ContactListener::player_ship_vs_wall_end(uintptr_t player_ship, uintptr_t wall) {
	ShipDataComponent* player = reinterpret_cast<ShipDataComponent*>(player_ship);
	player->is_colliding = false;
}

void ContactListener::enemy_ship_vs_wall_end(uintptr_t enemy_ship, uintptr_t wall) {

}

void ContactListener::player_projectile_vs_enemy_projectile_end(uintptr_t player_projectile, uintptr_t enemy_projectile) {

}



void ContactListener::add_floating_text_particle(olc::vf2d pos, std::string text) {
	EntityManager* entity_manager = GlobalEntityManager::get();
	AssetManager* asset_manager = GlobalAssetManager::get();

	Entity& entity = entity_manager->create_entity();
	// TODO: animation will be unused
	AnimationGroup animation_group = asset_manager->get_animation_group(AnimationGroupName::PARTICLE_EXPLOSION);
	ParticleDataComponent& new_data_component = entity.add_data_component<ParticleDataComponent>(pos, animation_group);
	ParticleCodeComponent& new_code_component = entity.add_code_component<ParticleCodeComponent>(new_data_component);
	new_data_component.text = text;
	new_data_component.particle_type = ParticleDataComponent::ParticleType::FLOATING_TEXT;
}