#include "PhysicsEngineDebugDrawer.h"


void PhysicsEngineDebugDrawer::init(olc::PixelGameEngine* pge, olc::TransformedView* transformed_view, float world_scale) {
	this->pge = pge;
	this->transformed_view = transformed_view;
	this->world_scale = world_scale;

	SetFlags(b2Draw::e_shapeBit);
}

void PhysicsEngineDebugDrawer::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
	olc::Pixel _color{
		static_cast<uint8_t>(color.r * 255.0f),
			static_cast<uint8_t>(color.g * 255.0f),
			static_cast<uint8_t>(color.b * 255.0f),
			0xFF
	};

	//olc::vf2d pos1{ vertices[vertexCount - 1].x, vertices[vertexCount - 1].y };
	//olc::vf2d pos2{ vertices[0].x, vertices[0].y };
	//pos1 /= world_scale;
	//pos2 /= world_scale;

	//for (int32 i = 1; i <= vertexCount; ++i) {
	//	transformed_view->DrawLineDecal(pos1, pos2, _color);

	//	pos1 = pos2;
	//	pos2 = {vertices[i].x, vertices[i].y};
	//	pos2 /= world_scale;
	//}


	for (int32 i = 1; i < vertexCount; ++i) {
		olc::vf2d pos1 = olc::vf2d{vertices[i - 1].x, vertices[i - 1].y};
		olc::vf2d pos2 = olc::vf2d{vertices[i].x, vertices[i].y};
		pos1 /= world_scale;
		pos2 /= world_scale;

		transformed_view->DrawLineDecal(pos1, pos2, _color);
	}

	olc::vf2d pos1 = olc::vf2d{vertices[0].x, vertices[0].y};
	olc::vf2d pos2 = olc::vf2d{vertices[vertexCount - 1].x, vertices[vertexCount - 1].y};
	pos1 /= world_scale;
	pos2 /= world_scale;

	transformed_view->DrawLineDecal(pos1, pos2, _color);
}

void PhysicsEngineDebugDrawer::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
	DrawPolygon(vertices, vertexCount, color);
}

void PhysicsEngineDebugDrawer::DrawCircle(const b2Vec2& center, float radius, const b2Color& color) {
	const int32_t segments = 16; // Adjust the number of segments as needed
	std::vector<b2Vec2> vertices;

	for (int32_t i = 0; i < segments; ++i) {
		float angle = static_cast<float>(i) / static_cast<float>(segments) * b2_pi * 2.0f;
		b2Vec2 vertex = center + b2Vec2(radius * cosf(angle), radius * sinf(angle));
		vertices.push_back(vertex);
	}

	DrawPolygon(vertices.data(), segments, color);
}

void PhysicsEngineDebugDrawer::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) {
	DrawCircle(center, radius, color);
}

void PhysicsEngineDebugDrawer::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {
	olc::Pixel _color{
		static_cast<uint8_t>(color.r * 255.0f),
			static_cast<uint8_t>(color.g * 255.0f),
			static_cast<uint8_t>(color.b * 255.0f),
			0xFF
	};

	olc::vf2d pos1{ p1.x, p1.y };
	olc::vf2d pos2{ p2.x, p2.y };
	pos1 /= world_scale;
	pos2 /= world_scale;

	transformed_view->DrawLineDecal(pos1, pos2, _color);
}

void PhysicsEngineDebugDrawer::DrawTransform(const b2Transform& xf) {
	b2Vec2 p1 = xf.p; // Position of the transformation
	b2Vec2 p2 = p1 + 0.5f * xf.q.GetXAxis(); // Endpoint of the x-axis line
	b2Vec2 p3 = p1 + 0.5f * xf.q.GetYAxis(); // Endpoint of the y-axis line

	olc::Pixel color_x{ 255, 0, 0, 255 }; // Red for x-axis
	olc::Pixel color_y{ 0, 255, 0, 255 }; // Green for y-axis
	olc::Pixel color_p{ 0, 0, 255, 255 }; // Blue for position point

	olc::vf2d p1_vf2d{ p1.x, p1.y };
	olc::vf2d p2_vf2d{ p2.x, p2.y };
	olc::vf2d p3_vf2d{ p3.x, p3.y };

	p1_vf2d /= world_scale;
	p2_vf2d /= world_scale;
	p3_vf2d /= world_scale;

	transformed_view->DrawLineDecal(p1_vf2d, p2_vf2d, color_x);
	transformed_view->DrawLineDecal(p1_vf2d, p3_vf2d, color_y);

	b2Color color = b2Color{
		(float)color_p.r / 255.0f,
		(float)color_p.g / 255.0f,
		(float)color_p.b / 255.0f,
		(float)color_p.a / 255.0f
	};

	DrawPoint(p1, 1.0f, color);
}

void PhysicsEngineDebugDrawer::DrawPoint(const b2Vec2& p, float size, const b2Color& color) {
	olc::Pixel _color{
		static_cast<uint8_t>(color.r * 255.0f),
			static_cast<uint8_t>(color.g * 255.0f),
			static_cast<uint8_t>(color.b * 255.0f),
			0xFF
	};

	olc::vf2d pos{ p.x, p.y };
	pos /= world_scale;

	transformed_view->DrawRectDecal(pos, olc::vf2d{2, 2}, _color);
}