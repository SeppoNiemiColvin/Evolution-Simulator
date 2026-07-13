#ifndef REGION_H_10832
#define REGION_H_10832

#include <Eigen/Dense>
class Creature;
using Eigen::Vector2d;

class Region {
public:
	virtual Vector2d get_nearest(Vector2d position) = 0 ;
	virtual void push_out(Creature* creature) = 0;
	virtual bool is_inside(Vector2d position) = 0;
};

#endif
