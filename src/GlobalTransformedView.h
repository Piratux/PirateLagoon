#pragma once

#include "olcPGEX_TransformedView.h"

class GlobalTransformedView {
public:
	static olc::TransformedView* transformed_view;

public:
	static void set(olc::TransformedView* transformed_view);
	static olc::TransformedView* get();
};