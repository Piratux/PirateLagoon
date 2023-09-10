#include "PhysicsEngine.h"
#include "GlobalTransformedView.h"
#include "GlobalPixelGameEngine.h"

PhysicsEngine::~PhysicsEngine() {
	std::cout << "Physics engine destructor called\n";
}

void PhysicsEngine::init(float time_step, int32 velocity_iterations, int32 position_iterations) {
	this->time_step = time_step;
	this->velocity_iterations = velocity_iterations;
	this->position_iterations = position_iterations;

	olc::PixelGameEngine* pge = GlobalPixelGameEngine::get();
	olc::TransformedView* transformed_view = GlobalTransformedView::get();

	debug_drawer.init(pge, transformed_view, world_scale);
	world.SetDebugDraw(&debug_drawer);
}

void PhysicsEngine::update() {
	world.Step(time_step, velocity_iterations, position_iterations);
}

void PhysicsEngine::draw() {
	world.DebugDraw();
}

b2Body* PhysicsEngine::create_body(b2BodyType body_type, float pos_x, float pos_y, bool fixed_rotation) {
	b2BodyDef body_def;
	body_def.type = body_type;
	body_def.position.Set(pos_x * world_scale, pos_y * world_scale);
	body_def.fixedRotation = fixed_rotation;
	body_def.linearDamping = 0.8f;
	body_def.angularDamping = 0.8f;
	b2Body* body = world.CreateBody(&body_def);
	return body;
}

void PhysicsEngine::destroy_body(b2Body* body) {
	if (world.IsLocked()) {
		std::cout << "Can't destroy body when world is locked\n";
		return;
	}

	world.DestroyBody(body);
}

void PhysicsEngine::add_collider(b2Body* body, ColliderData* data, b2Shape* shape) {
	b2FixtureDef fixture_def;

	switch (body->GetType()) {
	case b2BodyType::b2_staticBody:
		fixture_def.density = 0.0f;
		break;

	case b2BodyType::b2_kinematicBody:
		fixture_def.density = 1.0f;
		break;

	case b2BodyType::b2_dynamicBody:
		fixture_def.density = 1.0f;
		break;
	}

	fixture_def.shape = shape;
	fixture_def.friction = data->friction;
	fixture_def.userData.pointer = data->user_data;
	fixture_def.filter.categoryBits = data->category;
	fixture_def.filter.maskBits = data->mask;
	fixture_def.isSensor = data->is_sensor;

	body->CreateFixture(&fixture_def);
}

void PhysicsEngine::add_rectangle_collider(b2Body* body, ColliderData* data, float size_x, float size_y) {
	b2PolygonShape rect;
	rect.SetAsBox((size_x * world_scale) / 2.0f, (size_y * world_scale) / 2.0f, b2Vec2{ data->offset_x * world_scale, data->offset_y * world_scale }, 0.0f);
	add_collider(body, data, &rect);
}

void PhysicsEngine::add_circle_collider(b2Body* body, ColliderData* data, float radius) {
	b2CircleShape circle;
	circle.m_radius = radius * world_scale;
	circle.m_p = b2Vec2{ data->offset_x * world_scale, data->offset_y * world_scale };
	add_collider(body, data, &circle);
}

olc::vf2d PhysicsEngine::get_position(b2Body* body) {
	b2Vec2 new_pos = body->GetPosition();
	olc::vf2d temp_pos = { new_pos.x, new_pos.y };
	temp_pos /= world_scale;
	return temp_pos;
}

void PhysicsEngine::set_transform(b2Body* body, float pos_x, float pos_y, float angle) {
	b2Vec2 new_pos = b2Vec2{ pos_x * world_scale, pos_y * world_scale };
	body->SetTransform(new_pos, angle);
}

float PhysicsEngine::get_angle_radians(b2Body* body) {
	return body->GetAngle();
}

void PhysicsEngine::set_contact_listener(b2ContactListener* contact_listener) {
	world.SetContactListener(contact_listener);
}