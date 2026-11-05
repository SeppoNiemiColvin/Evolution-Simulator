#include "Prehash.h"
#include <cmath>

CoordPrehash hash2D::prehash(Vector2d point, double block_size) {
	return prehash(point(0), point(1), block_size);
}

CoordPrehash hash2D::prehash(double x_coord, double y_coord, double block_size) {
	const CoordPrehash x_coord_reduced{ static_cast<CoordPrehash>(floor(x_coord / block_size)) };
	const CoordPrehash y_coord_reduced{ static_cast<CoordPrehash>(floor(y_coord / block_size)) };
	return ((x_coord_reduced * 256) + y_coord_reduced);
}

bool hash2D::same_block(const double coord, const double other_coord, const double block_size) {
	const CoordPrehash coord_block{ static_cast<CoordPrehash>(floor(coord / block_size)) };
	const CoordPrehash other_block{ static_cast<CoordPrehash>(floor(other_coord / block_size))};
	return coord_block == other_block;
}