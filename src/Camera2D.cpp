#include "Camera2D.h"
#include "GlobalTransformedView.h"
#include "GlobalPixelGameEngine.h"
#include "Utils.h"
#include "PiraMath.h"

void Camera2D::update(float delta) {
	olc::TransformedView* transformed_view = GlobalTransformedView::get();
	olc::PixelGameEngine* pge = GlobalPixelGameEngine::get();

	// TEMP
	//if (!pge->GetKey(olc::SPACE).bHeld) {
		// "GetWorldVisibleArea()" expects that transformed view is initialised with window size
	if (follow_pos != nullptr) {

		olc::vf2d pos = *follow_pos - transformed_view->GetWorldVisibleArea() / 2.0f;

		apply_shake(pos, delta);

		// clamp position
		pos = pos.max(min_pos);
		pos = pos.min(max_pos - transformed_view->GetWorldVisibleArea());

		transformed_view->SetWorldOffset(pos);
	}

	//}

	current_zoom_scale = PiraMath::lerp(current_zoom_scale, target_zoom_scale, 2.0f * delta);
	transformed_view->SetWorldScale(olc::vf2d{current_zoom_scale, current_zoom_scale});
}

void Camera2D::draw() {
	return;
}

void Camera2D::set_follow_pos(olc::vf2d* follow_pos) {
	this->follow_pos = follow_pos;
}

void Camera2D::set_min_pos(olc::vf2d min_pos) {
	this->min_pos = min_pos;
}

void Camera2D::set_max_pos(olc::vf2d max_pos) {
	this->max_pos = max_pos;
}

void Camera2D::shake(olc::vf2d offset, float duration, float magnitude) {
	shake_offset = offset;
	shake_duration = std::max(duration, shake_duration);
	shake_magnitude = std::max(magnitude, shake_magnitude);
}

void Camera2D::small_shake(olc::vf2d offset) {
	shake(offset, 0.3f, 10);
}

void Camera2D::big_shake(olc::vf2d offset) {
	shake(offset, 0.6f, 20);
}

void Camera2D::toggle_zoom() {
	if (zoomed_out) {
		zoomed_out = false;
		target_zoom_scale = zoom_in_scale;
	}
	else {
		zoomed_out = true;
		target_zoom_scale = zoom_out_scale;
	}
}

void Camera2D::apply_shake(olc::vf2d& pos, float delta) {
	if (shake_duration <= 0.0f) {
		return;
	}

	shake_duration -= delta;

	olc::vf2d random_offset = olc::vf2d{ (Utils::random_float() - 0.5f) * shake_magnitude, (Utils::random_float() - 0.5f) * shake_magnitude };
	pos += random_offset;

	olc::vf2d direction_offset = shake_offset * shake_magnitude;
	pos += direction_offset;
}