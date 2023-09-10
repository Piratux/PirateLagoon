#include "Utils.h"
#include "GlobalTransformedView.h"
#include "PiraMath.h"

#include <iostream>

void Utils::check_null(void* ptr) {
	if (ptr == nullptr) {
		std::cout << "Error: Utils::check_null: global pointer is null." << std::endl;
		exit(-1);
	}
}

olc::vf2d Utils::b2Vec2_to_vf2d(b2Vec2 vec2) {
	return olc::vf2d{vec2.x, vec2.y};
}

b2Vec2 Utils::vf2d_to_b2Vec2(olc::vf2d vec2) {
	return b2Vec2{ vec2.x, vec2.y };
}

int Utils::random_int(int to) {
	return random_int(0, to);
}

int Utils::random_int(int from, int to) {
	return from + (std::rand() % (to - from + 1));
}

float Utils::random_float(float to) {
	return random_float(0.0f, to);
}

float Utils::random_float(float from, float to) {
	return from + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) / (to - from));
}

olc::vf2d Utils::random_direction(float from, float to) {
	return PiraMath::radians_to_vec2(random_float(from, to)).norm();
}

void Utils::draw_cirlce(olc::vf2d center, float radius, olc::Pixel color, int polygon_edges) {
	olc::TransformedView* transformed_view = GlobalTransformedView::get();

	std::vector<olc::vf2d> vertices;

	for (int32_t i = 0; i < polygon_edges; ++i) {
		float angle = static_cast<float>(i) / static_cast<float>(polygon_edges) * b2_pi * 2.0f;
		olc::vf2d vertex = center + olc::vf2d{radius* cosf(angle), radius* sinf(angle)};
		vertices.push_back(vertex);
	}

	for (int32 i = 1; i < vertices.size(); ++i) {
		olc::vf2d pos1 = vertices[i - 1];
		olc::vf2d pos2 = vertices[i];

		transformed_view->DrawLineDecal(pos1, pos2, color);
	}
	transformed_view->DrawLineDecal(vertices.back(), vertices.front(), color);
}