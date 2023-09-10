#include "ShipCodeComponent.h"
#include "GlobalPixelGameEngine.h"
#include "GlobalTransformedView.h"
#include "GlobalPhysicsEngine.h"
#include "GlobalAudioEngine.h"
#include "GlobalEntityManager.h"
#include "GlobalAssetManager.h"
#include "GlobalTileMap.h"
#include "GlobalCamera2D.h"
#include "GlobalGameState.h"
#include "PiraMath.h"
#include "EnumColliderFlag.h"
#include "ProjectileCodeComponent.h"
#include "ProjectileDataComponent.h"
#include "ParticleCodeComponent.h"
#include "ParticleDataComponent.h"
#include "Utils.h"
#include "Events.h"

ShipCodeComponent::ShipCodeComponent(ShipDataComponent& data_component, ShipDataComponent& player_data_component)
	: data_component(data_component)
	, player_data_component(player_data_component) {

}

bool ShipCodeComponent::update(float delta) {
	PhysicsEngine* physics_engine = GlobalPhysicsEngine::get();

	data_component.pos = physics_engine->get_position(data_component.body);

	data_component.animation_group.update(delta);

	update_timers(delta);
	update_animation();
	if (data_component.is_dead() && data_component.death_handled == false) {
		data_component.death_handled = true;
		ship_just_died();
	}

	switch (data_component.ship_type) {
	case ShipType::PLAYER:
		return update_as_player_ship(delta);
		break;
	default:
		return update_as_enemy_ship(delta);
		break;
	}
}

void ShipCodeComponent::draw() {
	olc::TransformedView* transformed_view = GlobalTransformedView::get();

	if (data_component.ship_type == ShipType::PLAYER) {
		draw_closest_enemy_direction_arrow();
	}


	const AnimationFrame& animation_frame = data_component.animation_group.get_frame();
	olc::Decal* decal = animation_frame.image->Decal();

	transformed_view->DrawPartialRotatedDecal(
		data_component.pos,
		decal,
		PiraMath::degrees_to_radians(data_component.get_rotation_degrees()),
		animation_frame.source_size / 2.0f,
		animation_frame.source_pos,
		animation_frame.source_size,
		data_component.texture_scale
	);


	if (data_component.ship_type == ShipType::PLAYER) {
		return;
	}

	// TEMP: debug info
	//TileMap* tile_map = GlobalTileMap::get();

	//olc::Pixel colour = olc::RED;
	//olc::vf2d ship_direction = PiraMath::degrees_to_vec2(data_component.get_rotation_degrees());
	//olc::vf2d ray_hit_pos = data_component.pos + ship_direction.norm() * data_component.avoid_range;
	//TileMapRaycastData raycast_data = tile_map->raycast(data_component.pos, ship_direction, data_component.avoid_range);
	//if (!raycast_data.hit) {
	//	colour = olc::GREEN;
	//}
	//else {
	//	ray_hit_pos = raycast_data.hit_pos;
	//	transformed_view->DrawRectDecal(raycast_data.tile_idx * tile_map->get_tile_size(), tile_map->get_tile_size(), colour);
	//}

	//transformed_view->DrawLineDecal(data_component.pos, ray_hit_pos, colour);

	//Utils::draw_cirlce(data_component.pos, data_component.combat_range, olc::DARK_YELLOW);
	//Utils::draw_cirlce(data_component.pos, data_component.attack_range, olc::DARK_YELLOW);
	//Utils::draw_cirlce(data_component.spawn_pos, data_component.patrol_range, olc::DARK_YELLOW);
	//Utils::draw_cirlce(data_component.spawn_pos, data_component.retreat_range, olc::DARK_YELLOW);
}

void ShipCodeComponent::update_closest_enemy_arrow(float delta) {
	olc::TransformedView* transformed_view = GlobalTransformedView::get();

	olc::vf2d nearest_enemy_pos = get_closest_enemy_pos();
	olc::vf2d vec_to_nearest_enemy = PiraMath::vec2_from_to(data_component.pos, nearest_enemy_pos);
	float distance_to_nearest_enemy = vec_to_nearest_enemy.mag();
	float visible_width = transformed_view->GetWorldVisibleArea().x;
	float half_visible_width = visible_width / 2.0f;

	if (distance_to_nearest_enemy < half_visible_width) {
		data_component.show_arrow = false;
	}
	else if (distance_to_nearest_enemy > visible_width) {
		data_component.show_arrow = true;
	}

	if (data_component.show_arrow) {
		data_component.nearest_enemy_arrow_alpha = PiraMath::lerp(data_component.nearest_enemy_arrow_alpha, 1.0f, 2.0f * delta);
	}
	else {
		data_component.nearest_enemy_arrow_alpha = PiraMath::lerp(data_component.nearest_enemy_arrow_alpha, 0.0f, 2.0f * delta);
	}
}

void ShipCodeComponent::draw_closest_enemy_direction_arrow() {
	olc::TransformedView* transformed_view = GlobalTransformedView::get();
	AssetManager* asset_manager = GlobalAssetManager::get();

	GameState* game_state = GlobalGameState::get();
	if (game_state->get() != GameState::State::ACTION) {
		return;
	}

	olc::vf2d nearest_enemy_pos = get_closest_enemy_pos();
	olc::vf2d vec_to_nearest_enemy = PiraMath::vec2_from_to(data_component.pos, nearest_enemy_pos);

	olc::Decal* decal = asset_manager->get_decal(ImageName::ARROW);
	olc::vf2d size = decal->sprite->Size();

	transformed_view->DrawPartialRotatedDecal(
		data_component.pos,
		decal,
		PiraMath::vec2_to_radians(vec_to_nearest_enemy),
		olc::vf2d{size.x / 2, size.y},
		olc::vf2d{0, 0},
		size,
		data_component.texture_scale,
		olc::PixelF(1.0f, 1.0f, 1.0f, data_component.nearest_enemy_arrow_alpha)
	);
}

olc::vf2d ShipCodeComponent::get_closest_enemy_pos() {
	EntityManager* entity_manager = GlobalEntityManager::get();

	float closest_distance = 1000000000.0f;
	olc::vf2d closest_pos = olc::vf2d{ 0,0 };
	auto& entities = entity_manager->get_entities();
	for (auto& entity : entities) {
		for (auto& entity_data_component : entity->data_components) {
			ShipDataComponent* enemy_data_component = dynamic_cast<ShipDataComponent*>(entity_data_component.get());
			if (enemy_data_component) {
				if (enemy_data_component->ship_type != ShipType::PLAYER) {
					if (enemy_data_component->is_dead() == false) {
						float distance = PiraMath::vec2_from_to(data_component.pos, enemy_data_component->pos).mag2();
						if (distance < closest_distance) {
							closest_distance = distance;
							closest_pos = enemy_data_component->pos;
						}
					}
				}
			}
		}
	}

	return closest_pos;
}

bool ShipCodeComponent::update_as_player_ship(float delta) {
	update_closest_enemy_arrow(delta);

	if (data_component.is_dead()) {
		return true;
	}

	olc::PixelGameEngine* pge = GlobalPixelGameEngine::get();

	// handle input
	if (!data_component.is_dead()) {
		// Ship rotation (the faster you move, the faster you turn)
		if (pge->GetKey(olc::RIGHT).bHeld || pge->GetKey(olc::D).bHeld) {
			data_component.rotate_ship(ShipSide::RIGHT, delta);
		}
		if (pge->GetKey(olc::LEFT).bHeld || pge->GetKey(olc::A).bHeld) {
			data_component.rotate_ship(ShipSide::LEFT, delta);
		}

		// Ship acceleration
		if (pge->GetKey(olc::UP).bHeld || pge->GetKey(olc::W).bHeld) {
			data_component.accelerate(1, delta);
		}
		if (pge->GetKey(olc::DOWN).bHeld || pge->GetKey(olc::S).bHeld) {
			data_component.accelerate(-1, delta);
		}

		olc::vf2d dir = PiraMath::degrees_to_vec2(data_component.get_rotation_degrees());

		dir *= data_component.get_velocity();

		data_component.body->SetLinearVelocity(b2Vec2{ dir.x, dir.y });


		// Shoot cannonballs from side
		if (pge->GetKey(olc::K).bHeld || pge->GetKey(olc::L).bHeld) {
			if (can_shoot()) {
				if (pge->GetKey(olc::K).bHeld) {
					shoot_as_player_ship(ShipSide::LEFT);
				}
				else {
					shoot_as_player_ship(ShipSide::RIGHT);
				}
			}
		}

		// Unstuck button
		if (pge->GetKey(olc::B).bPressed) {
			data_component.take_damage(data_component.get_max_health());
		}

		// Zoom in/out
		Camera2D* camera = GlobalCamera2D::get();
		if (pge->GetKey(olc::SPACE).bPressed) {
			camera->toggle_zoom();
		}
	}

	return true;
}

bool ShipCodeComponent::update_as_enemy_ship(float delta) {
	if (data_component.is_dead()) {
		if (data_component.dead_timer_seconds <= 0.01f) {
			Events::add(Events::Type::ENEMY_DESTROYED);
			return false;
		}
		else {
			return true;
		}
	}

	switch (data_component.ship_type) {
	case ShipType::ENEMY_BLUE:
		update_cannons_as_blue_ship(delta);
		break;
	case ShipType::ENEMY_BLACK:
		update_cannons_as_black_ship(delta);
		break;
	default:
		update_cannons_as_enemy_ship(delta);
		break;
	}

	update_movement_as_enemy_ship(delta);


	return true;
}

void ShipCodeComponent::update_movement_as_enemy_ship(float delta) {
	// TODO:
	// - set up last ship side rotation change timer to avoid jiggling rotation left/right
	// - choose random spawn point offset when retreating (and keep changing it slightly while retreating, so it won't go in straight line)
	// - choose a side when rotating

	// Enemy pathing:
	// - When travelling to destination(chasing player or going home), prioritise obstacle
	// avoidance, otherwise use pathfinding
	// - When fighting player, if there are immovable obstacles or terrain between cannon
	// and player, use pathfinding until path between cannon and player is clear. Use
	// obstacle and terrain avoidance.
	// - When using pathfinding, only use tiles for path that are at least 1 tile away from
	// nearest shore
	// - When terrain or obstacle is unavoidable with current speed, slow down and turn


	// TODO: if player slowed down, enemy should maybe slow down too
	TileMap* tile_map = GlobalTileMap::get();



	olc::vf2d ship_direction = PiraMath::degrees_to_vec2(data_component.get_rotation_degrees());

	data_component.body->SetLinearVelocity(Utils::vf2d_to_b2Vec2(ship_direction * data_component.get_velocity()));


	// reduce the chance of enemy ship getting stuck turning left/right
	if (data_component.just_stopped_avoiding_terrain) {
		if (terrain_in_front(ship_direction, data_component.avoid_angle)) {
			avoid_obstacles(false);
		}
	}

	// continue avoiding obstacles for a bit by continue turning
	if (data_component.rotation_timer_seconds > 0.01f) {
		if (terrain_in_front(ship_direction, data_component.avoid_angle)) {
			data_component.accelerate(1, delta);
		}
		else {
			data_component.accelerate(1, delta);
		}
		data_component.rotate_ship(data_component.ship_side, delta);
		return;
	}

	// if there is something in front of ship, use obstacle avoidance
	if (obstacles_in_front(ship_direction, data_component.avoid_angle)
		|| terrain_in_front(ship_direction, data_component.avoid_angle)
		) {
		avoid_obstacles();
		return;
	}


	// Go towards spawn position if
	// - already retreating
	// - ship went too far
	// - player too far
	// - player died
	olc::vf2d dir_to_player = PiraMath::vec2_from_to(data_component.pos, player_data_component.pos);
	olc::vf2d dir_to_spawn = PiraMath::vec2_from_to(data_component.pos, data_component.spawn_pos);
	float distance_to_player = dir_to_player.mag();
	float distance_to_spawn = dir_to_spawn.mag();
	if (data_component.is_retreating
		|| distance_to_spawn > data_component.patrol_range
		|| distance_to_player > data_component.attack_range
		|| player_data_component.is_dead()
		) {
		data_component.is_retreating = true;

		// if there is terrain in between ship and "spawn_pos", use path finding
		if (tile_map->raycast(data_component.pos, dir_to_spawn, data_component.avoid_range).hit) {
			use_pathfinding(delta);
			return;
		}

		float det = PiraMath::vec2_det(dir_to_spawn, ship_direction);
		float dot = PiraMath::vec2_dot(dir_to_spawn, ship_direction);

		// Handle special case when ship angle between current direction and
		// direction to spawn pos is 180 degrees by forcing ship to turn a bit.
		// Resulting sign is multiplied by -1, because direction rotation is rotated by 180 degrees
		int rotation_side = (det == 0 && dot > 0) ? 1 : -PiraMath::sign(det);


		data_component.rotate_ship(rotation_side == -1 ? ShipSide::LEFT : ShipSide::RIGHT, delta);

		// When ship is close enough to spawn position, stop saying it's "retreating"
		if (PiraMath::vec2_from_to(data_component.pos, data_component.spawn_pos).mag() < data_component.retreat_range) {
			data_component.is_retreating = false;
		}
	}
	// Fight the player
	else {
		// if there is terrain in between ship and player, use path finding
		if (tile_map->raycast(data_component.pos, dir_to_player, dir_to_player.mag()).hit) {
			use_pathfinding(delta);
			return;
		}

		// if player is too far, try to get closer to it first
		float angle_to_player = PiraMath::angle_between_vec2_degrees(ship_direction, dir_to_player);
		if (PiraMath::vec2_from_to(data_component.pos, player_data_component.pos).mag() > data_component.combat_range) {
			data_component.rotate_ship(angle_to_player > 0.0f ? ShipSide::RIGHT : ShipSide::LEFT, delta);
		}
		// Otherwise face its sides towards player
		else {
			if (angle_to_player > 0.0f) {
				if (angle_to_player < 90.0f) {
					data_component.ship_side = ShipSide::LEFT;
				}
				else if (angle_to_player > 90.0f) {
					data_component.ship_side = ShipSide::RIGHT;
				}
			}
			else {
				if (angle_to_player > -90.0f) {
					data_component.ship_side = ShipSide::RIGHT;
				}
				else if (angle_to_player < -90.0f) {
					data_component.ship_side = ShipSide::LEFT;
				}
			}
			data_component.rotate_ship(data_component.ship_side, delta);
		}
	}


	// Ship constantly accelerates, when it doesn't need to do obstacle avoidance and pathfinding
	data_component.accelerate(1.0f, delta);
}

void ShipCodeComponent::update_cannons_as_enemy_ship(float delta) {
	if (data_component.is_dead() || player_data_component.is_dead()) {
		return;
	}

	if (!can_shoot()) {
		return;
	}

	// this prevents all ships shooting at the same time
	olc::vf2d dir_to_player = PiraMath::vec2_from_to(data_component.pos, player_data_component.pos);
	float distance_to_player = dir_to_player.mag();
	if (distance_to_player > data_component.attack_range) {
		return;
	}

	data_component.reset_reload_timer();
	shoot_as_enemy_ship(get_player_side());

	//game_manager.audio_player.play_sound("cannon_blast");
}

void ShipCodeComponent::update_cannons_as_blue_ship(float delta) {
	if (data_component.is_dead() || player_data_component.is_dead()) {
		return;
	}

	// this prevents all ships shooting at the same time
	olc::vf2d dir_to_player = PiraMath::vec2_from_to(data_component.pos, player_data_component.pos);
	float distance_to_player = dir_to_player.mag();
	if (distance_to_player > data_component.attack_range) {
		return;
	}

	if (can_shoot() || data_component.cannonball_barrage_amount_left > 0) {
		if (can_shoot()) {
			data_component.cannonball_barrage_amount_left = data_component.cannonball_barrage_amount_total;
			data_component.reset_reload_timer();
		}

		if (data_component.cannoball_barrage_cooldown_timer_seconds <= 0.01f) {
			data_component.cannonball_barrage_amount_left--;
			data_component.cannoball_barrage_cooldown_timer_seconds = data_component.cannonball_barrage_interval_seconds;

			shoot_as_enemy_ship(get_player_side());
		}
	}
}

void ShipCodeComponent::update_cannons_as_black_ship(float delta) {
	// TODO: when audio is added, this could produce big blast sound
	update_cannons_as_enemy_ship(delta);
}

void ShipCodeComponent::shoot_as_player_ship(ShipSide ship_side) {
	AudioEngine* audio_engine = GlobalAudioEngine::get();

	audio_engine->play(AudioName::CANNON_BLAST);

	data_component.reset_reload_timer();
	data_component.reset_regeneration_timer();

	ColliderData ball_collider_data;
	ball_collider_data.category = ColliderFlag::PLAYER_PROJECTILE;
	ball_collider_data.mask = ColliderFlag::WALL | ColliderFlag::ENEMY_SHIP;

	shoot_cannon_ball(ship_side, ball_collider_data);
}

void ShipCodeComponent::shoot_as_enemy_ship(ShipSide ship_side) {
	ColliderData ball_collider_data;
	ball_collider_data.category = ColliderFlag::ENEMY_PROJECTILE;
	ball_collider_data.mask = ColliderFlag::WALL | ColliderFlag::PLAYER_SHIP;

	shoot_cannon_ball(ship_side, ball_collider_data);
}

void ShipCodeComponent::add_death_explosion() {
	EntityManager* entity_manager = GlobalEntityManager::get();
	AssetManager* asset_manager = GlobalAssetManager::get();

	Entity* entity = entity_manager->get_entity(data_component.entity_id);
	if (entity != nullptr) {
		AnimationGroup animation_group = asset_manager->get_animation_group(AnimationGroupName::PARTICLE_EXPLOSION);
		animation_group.set_looping(false);
		ParticleDataComponent& new_data_component = entity->add_data_component<ParticleDataComponent>(data_component.pos, animation_group);
		ParticleCodeComponent& new_code_component = entity->add_code_component<ParticleCodeComponent>(new_data_component);

		new_data_component.follow_pos = &data_component.pos;
		new_data_component.particle_type = ParticleDataComponent::ParticleType::EXPLOSION;

		// TODO: set explosion scale based on ship size
		// float explosion_scale = 0.2f + temp_projectile.texture_scale.x * 0.2f;
	}
}

void ShipCodeComponent::add_flying_crew_members() {
	EntityManager* entity_manager = GlobalEntityManager::get();
	AssetManager* asset_manager = GlobalAssetManager::get();
	PhysicsEngine* physics_engine = GlobalPhysicsEngine::get();

	int amount = Utils::random_int(1, 3);
	for (int i = 0; i < amount; i++) {
		Entity& entity = entity_manager->create_entity();
		AnimationGroup animation_group = asset_manager->get_animation_group(AnimationGroupName::PARTICLE_FLYING_CREW_MEMBER);
		ParticleDataComponent& new_data_component = entity.add_data_component<ParticleDataComponent>(data_component.pos, animation_group);
		ParticleCodeComponent& new_code_component = entity.add_code_component<ParticleCodeComponent>(new_data_component);
		new_data_component.texture_scale = olc::vf2d{ 5.0f, 5.0f };
		new_data_component.lifespan_seconds = 1.5f;
		new_data_component.particle_type = ParticleDataComponent::ParticleType::FLYING_CREW_MEMBER;
		new_data_component.body = physics_engine->create_body(b2BodyType::b2_dynamicBody, data_component.pos.x, data_component.pos.y);
		new_data_component.body->SetLinearVelocity(data_component.body->GetLinearVelocity());
		new_data_component.body->ApplyForce(
			Utils::vf2d_to_b2Vec2(Utils::random_direction() * Utils::random_float(2.0f, 4.0f)),
			Utils::vf2d_to_b2Vec2(physics_engine->get_position(new_data_component.body)),
			true
		);

		ColliderData crew_particle_collider_data;
		crew_particle_collider_data.category = ColliderFlag::CREW_PARTICLE;
		crew_particle_collider_data.mask = 0;
		crew_particle_collider_data.is_sensor = true;

		physics_engine->add_circle_collider(
			new_data_component.body,
			&crew_particle_collider_data,
			10.0f
		);

		int random_sprite_idx = Utils::random_int(1, 6);
		switch (random_sprite_idx) {
		case 1:
			new_data_component.animation_group.play_animation(AnimationName::FLYING_CREW_MEMBER_1);
			break;
		case 2:
			new_data_component.animation_group.play_animation(AnimationName::FLYING_CREW_MEMBER_2);
			break;
		case 3:
			new_data_component.animation_group.play_animation(AnimationName::FLYING_CREW_MEMBER_3);
			break;
		case 4:
			new_data_component.animation_group.play_animation(AnimationName::FLYING_CREW_MEMBER_4);
			break;
		case 5:
			new_data_component.animation_group.play_animation(AnimationName::FLYING_CREW_MEMBER_5);
			break;
		case 6:
			new_data_component.animation_group.play_animation(AnimationName::FLYING_CREW_MEMBER_6);
			break;
		default:
			break;
		}
	}
}

bool ShipCodeComponent::can_shoot() {
	return data_component.reload_timer_seconds <= 0.01f;
}

void ShipCodeComponent::update_timers(float delta) {
	data_component.reload_timer_seconds = std::max(data_component.reload_timer_seconds - delta, 0.0f);
	data_component.no_regeneration_timer_seconds = std::max(data_component.no_regeneration_timer_seconds - delta, 0.0f);
	data_component.cannoball_barrage_cooldown_timer_seconds = std::max(data_component.cannoball_barrage_cooldown_timer_seconds - delta, 0.0f);

	float before_rotation_timer_seconds = data_component.rotation_timer_seconds;
	data_component.rotation_timer_seconds = std::max(data_component.rotation_timer_seconds - delta, 0.0f);
	if (before_rotation_timer_seconds >= 0.01f && data_component.rotation_timer_seconds < 0.01f) {
		data_component.just_stopped_avoiding_terrain = true;
	}
	else {
		data_component.just_stopped_avoiding_terrain = false;
	}

	if (data_component.is_colliding == false) {
		data_component.no_ram_damage_timer_seconds = std::max(data_component.no_ram_damage_timer_seconds - delta, 0.0f);
	}

	if (data_component.is_dead()) {
		data_component.dead_timer_seconds = std::max(data_component.dead_timer_seconds - delta, 0.0f);
	}

	if (!data_component.is_dead()
		&& data_component.get_health() != data_component.get_max_health()
		&& data_component.ship_type == ShipType::PLAYER
		) {
		if (data_component.no_regeneration_timer_seconds < 0.01f) {
			int amount_healed = data_component.heal((int)std::round((data_component.regeneration_percent / 100.0f) * (float)data_component.get_max_health()));
			data_component.no_regeneration_timer_seconds = data_component.regeneration_speed_seconds;

			if (amount_healed > 0) {
				// floating text particle code copied from ContactListener
				// TODO: don't duplicate this
				EntityManager* entity_manager = GlobalEntityManager::get();
				AssetManager* asset_manager = GlobalAssetManager::get();

				Entity& entity = entity_manager->create_entity();
				AnimationGroup animation_group = asset_manager->get_animation_group(AnimationGroupName::PARTICLE_EXPLOSION); // TODO: animation will be unused
				ParticleDataComponent& new_data_component = entity.add_data_component<ParticleDataComponent>(data_component.pos, animation_group);
				ParticleCodeComponent& new_code_component = entity.add_code_component<ParticleCodeComponent>(new_data_component);
				new_data_component.text = "+" + std::to_string(amount_healed);
				new_data_component.text_color = olc::DARK_GREEN;
				new_data_component.particle_type = ParticleDataComponent::ParticleType::FLOATING_TEXT;
			}
		}
		else {
			data_component.no_regeneration_timer_seconds = std::max(data_component.no_regeneration_timer_seconds - delta, 0.0f);
		}
	}
}

void ShipCodeComponent::update_animation() {
	float health_percentage = data_component.get_health_percentage();

	if (health_percentage >= 66.6f) {
		data_component.animation_group.play_animation(AnimationName::SHIP_HEALTHY);
	}
	else if (health_percentage >= 33.3f) {
		data_component.animation_group.play_animation(AnimationName::SHIP_DAMAGED_1);
	}
	else if (health_percentage > 0.0f) {
		data_component.animation_group.play_animation(AnimationName::SHIP_DAMAGED_2);
	}
	else {
		data_component.animation_group.play_animation(AnimationName::SHIP_DESTROYED);
	}
}

void ShipCodeComponent::shoot_cannon_ball(ShipSide ship_side, ColliderData& ball_collider_data) {
	EntityManager* entity_manager = GlobalEntityManager::get();
	AssetManager* asset_manager = GlobalAssetManager::get();
	PhysicsEngine* physics_engine = GlobalPhysicsEngine::get();

	float degrees = ship_side == ShipSide::RIGHT
		? 90 + data_component.get_rotation_degrees()
		: 270 + data_component.get_rotation_degrees();
	olc::vf2d projectile_direction = PiraMath::degrees_to_vec2(degrees);

	olc::vf2d projectile_start_pos = data_component.pos + projectile_direction * data_component.cannon_ball_pos_offset_scale * data_component.texture_scale.x;

	Entity& entity = entity_manager->create_entity();

	AnimationGroup animation_group = asset_manager->get_animation_group(AnimationGroupName::PROJECTILE_CANNON_BALL);
	ProjectileDataComponent& new_data_component = entity.add_data_component<ProjectileDataComponent>(projectile_start_pos, animation_group);
	ProjectileCodeComponent& new_code_component = entity.add_code_component<ProjectileCodeComponent>(new_data_component);

	new_data_component.damage = data_component.damage;
	new_data_component.texture_scale = data_component.texture_scale;
	new_data_component.body = physics_engine->create_body(b2BodyType::b2_dynamicBody, projectile_start_pos.x, projectile_start_pos.y);
	new_data_component.body->SetLinearVelocity(data_component.body->GetLinearVelocity());
	new_data_component.body->ApplyForce(
		Utils::vf2d_to_b2Vec2(projectile_direction * data_component.cannon_strength * (data_component.texture_scale.x * data_component.texture_scale.x)), // squared because collider radius increases by squared
		Utils::vf2d_to_b2Vec2(physics_engine->get_position(new_data_component.body)),
		true
	);

	ball_collider_data.user_data = reinterpret_cast<uintptr_t>(static_cast<DataComponent*>(&new_data_component));
	physics_engine->add_circle_collider(
		new_data_component.body,
		&ball_collider_data,
		data_component.texture_scale.x * (animation_group.get_frame().source_size.x / 2.0f)
	);


	// create trailing particle
	AnimationGroup particle_animation_group = asset_manager->get_animation_group(AnimationGroupName::PARTICLE_CANNON_BALL_TRAIL);
	ParticleDataComponent& particle_data_component = entity.add_data_component<ParticleDataComponent>(projectile_start_pos, particle_animation_group);
	ParticleCodeComponent& particle_code_component = entity.add_code_component<ParticleCodeComponent>(particle_data_component);
	particle_data_component.start_pos = projectile_start_pos;
	particle_data_component.follow_body = new_data_component.body;
	particle_data_component.follow_object_texture_width = (int)animation_group.get_frame().source_size.x;
	particle_data_component.particle_type = ParticleDataComponent::ParticleType::TRAIL;
	particle_data_component.cannon_ball_scale = data_component.texture_scale.x;
}

bool ShipCodeComponent::obstacles_in_front(olc::vf2d dir, float cone_angle_degrees) {
	return false;
}

bool ShipCodeComponent::terrain_in_front(olc::vf2d dir, float cone_angle_degrees) {
	TileMap* tile_map = GlobalTileMap::get();
	return tile_map->raycast(data_component.pos, dir, data_component.avoid_range).hit;
}

void ShipCodeComponent::avoid_obstacles(bool switch_sides) {
	data_component.rotation_timer_seconds = calculate_rotation_timer();
	if (switch_sides) {
		data_component.ship_side = Utils::random_int() == 1 ? ShipSide::RIGHT : ShipSide::LEFT;
	}
}

void ShipCodeComponent::use_pathfinding(float delta) {
	data_component.accelerate(1, delta);
}

float ShipCodeComponent::calculate_rotation_timer() {
	return 100.0f / data_component.rotation_speed + Utils::random_float(0.5f);
}

void ShipCodeComponent::ship_just_died() {
	if (data_component.ship_type == ShipType::PLAYER) {
		Events::add(Events::Type::PLAYER_DESTROYED);
	}

	add_death_explosion();
	add_flying_crew_members();
}

ShipSide ShipCodeComponent::get_player_side() {
	// TODO: rewrite this using angle_between_vec2_radians
	olc::vf2d ship_direction = Utils::b2Vec2_to_vf2d(data_component.body->GetLinearVelocity());
	olc::vf2d right_direction = PiraMath::vec2_rotate_degrees(ship_direction, 90);
	olc::vf2d left_direction = PiraMath::vec2_rotate_degrees(ship_direction, -90);

	float right_side_distance = PiraMath::vec2_from_to(data_component.pos + right_direction, player_data_component.pos).mag2();
	float left_side_distance = PiraMath::vec2_from_to(data_component.pos + left_direction, player_data_component.pos).mag2();

	return left_side_distance < right_side_distance ? ShipSide::LEFT : ShipSide::RIGHT;
}