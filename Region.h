#ifndef REGION_H_10832
#define REGION_H_10832

#include <Eigen/Dense>
class Creature;
using Eigen::Vector2d;

class Region {
public:
	virtual Vector2d get_nearest(const Vector2d position) const = 0 ;
	virtual void push_out(Creature* creature) const = 0;
	virtual bool is_inside(const Vector2d position) const = 0;
	virtual double get_distance(const Vector2d position) const { return (get_nearest(position) - position).norm(); }
};

#endif
