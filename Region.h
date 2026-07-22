#ifndef REGION_H_10832
#define REGION_H_10832

#include <Eigen/Dense>
class Creature;

using Eigen::Vector2d;

class Region {
public:
	virtual Vector2d get_nearest(const Vector2d& position) const = 0 ;
	//pushes a creature that is inside the region outside
	virtual void push_out(Creature* creature) const = 0;
	//pulls a creature that is outside the region in
	virtual void pull_inside(Creature* creature) const;
	virtual bool is_inside(const Vector2d& position) const = 0;
	virtual double get_distance(const Vector2d& position) const {
		return (get_nearest(position) - position).norm();
	}
	//a return of -1 will signify that area is not a valid quiery for htis type of region
	virtual double get_area() const { return -1.0; }
	//returns a random point in the region
	virtual Vector2d random_point() const = 0;
};



#endif
