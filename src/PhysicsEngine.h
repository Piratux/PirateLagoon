// TODO: 
// - add parameter overloads with vf2d types
// - pass collider in const ref instead

#pragma once

#include "box2d/box2d.h"
#include "olcPixelGameEngine.h"
#include "ColliderData.h"
#include "PhysicsEngineDebugDrawer.h"

class ContactListener;

// physics engine (box2d wrapper)
class PhysicsEngine {
private:
	// When bodies are created, their positions are multiplied by this number.
	// According to docs, to ensure stable physics, positions should be within 0.1 - 10 meters
	// https://box2d.org/documentation/md__d_1__git_hub_box2d_docs__f_a_q.html
	const float world_scale = 0.01f; // should be tuned so that 
	b2Vec2 gravity{ 0.0f, 0.0f };
	b2World world{ gravity };

	float time_step = 1.0f / 60.0f;
	int32 velocity_iterations = 6;
	int32 position_iterations = 2;

	PhysicsEngineDebugDrawer debug_drawer;

public:
	~PhysicsEngine();

	void init(float time_step, int32 velocity_iterations = 6, int32 position_iterations = 2);
	void update();
	void draw();

	b2Body* create_body(b2BodyType body_type, float pos_x, float pos_y, bool fixed_rotation = true);
	void destroy_body(b2Body* body);

	void add_collider(b2Body* body, ColliderData* data, b2Shape* shape);
	void add_rectangle_collider(b2Body* body, ColliderData* data, float size_x, float size_y);
	void add_circle_collider(b2Body* body, ColliderData* data, float radius);

	olc::vf2d get_position(b2Body* body);
	void set_transform(b2Body* body, float pos_x, float pos_y, float angle = 0.0f);
	float get_angle_radians(b2Body* body);

	void set_contact_listener(b2ContactListener* contact_listener);
};