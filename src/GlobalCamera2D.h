#pragma once
#include "Camera2D.h"

class GlobalCamera2D {
public:
	static Camera2D* camera_2d;

public:
	static void set(Camera2D* camera_2d);
	static Camera2D* get();
};
