#include "GlobalTransformedView.h"
#include "Utils.h"

olc::TransformedView* GlobalTransformedView::transformed_view = nullptr;

void GlobalTransformedView::set(olc::TransformedView* transformed_view) {
	Utils::check_null(transformed_view);
	GlobalTransformedView::transformed_view = transformed_view;
}

olc::TransformedView* GlobalTransformedView::get() {
	Utils::check_null(GlobalTransformedView::transformed_view);
	return GlobalTransformedView::transformed_view;
}
