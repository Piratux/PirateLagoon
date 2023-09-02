#include "ExampleCodeComponent.h"
#include "GlobalTransformedView.h"
#include "GlobalPhysicsEngine.h"
#include "PiraMath.h"

ExampleCodeComponent::ExampleCodeComponent(ExampleDataComponent& data_component)
	: data_component(data_component) {

}

bool ExampleCodeComponent::update(float delta) {
	PhysicsEngine* physics_engine = GlobalPhysicsEngine::get();

	data_component.pos = physics_engine->get_position(data_component.body);

	data_component.animation_group.update(delta);

	return true;
}

void ExampleCodeComponent::draw() {
	olc::TransformedView* transformed_view = GlobalTransformedView::get();

	const AnimationFrame& animation_frame = data_component.animation_group.get_frame();
	olc::Decal* decal = animation_frame.image->Decal();

	transformed_view->DrawPartialRotatedDecal(
		data_component.pos,
		decal,
		PiraMath::degrees_to_radians(data_component.get_rotation_degrees()),
		animation_frame.source_size / 2.0f,
		animation_frame.source_pos,
		animation_frame.source_size
	);
}