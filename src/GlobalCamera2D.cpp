#include "GlobalCamera2D.h"
#include "Utils.h"

Camera2D* GlobalCamera2D::camera_2d = nullptr;

void GlobalCamera2D::set(Camera2D* camera_2d) {
	Utils::check_null(camera_2d);
	GlobalCamera2D::camera_2d = camera_2d;
}

Camera2D* GlobalCamera2D::get() {
	Utils::check_null(GlobalCamera2D::camera_2d);
	return GlobalCamera2D::camera_2d;
}