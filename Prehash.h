#ifndef PREHASH2081
#define PREHASH2081

#include <Eigen/Dense>
using Eigen::Vector2d;
using CoordPrehash = long;

namespace hash2D {
	CoordPrehash prehash(Vector2d point, double block_size);
	CoordPrehash prehash(double x_coord, double y_coord, double block_size);
	bool same_block(const double coord, const double other_coord, const double block_size);
}



#endif