#ifndef REGION_H_10832
#define REGION_H_10832

#include <Eigen/Dense>

/*
* An abstract class/interface to hold a 2d shape
* Useful to describe things that take up space and provide obstacles for movement
*/
class Creature;
class Rectangle;

using Eigen::Vector2d;

class Region {
public:
	//gets the point in the region that is closet to the point position
	virtual Vector2d get_nearest(const Vector2d& position) const = 0;

	//pushes a creature that is inside the region outside
	virtual void push_out(Creature* creature) const = 0;

	//pulls a creature that is outside the region in
	virtual void pull_inside(Creature* creature) const;

	//returns whether this point is in this poistion
	virtual bool is_inside(const Vector2d& position) const = 0;

	//distance from the point position to the region
	virtual double get_distance(const Vector2d& position) const {
		return (get_nearest(position) - position).norm();
	}

	//a return of -1 will signify that area is not a valid quiery for this type of region
	virtual double get_area() const { return -1.0; }

	//returns a random point in the region
	virtual Vector2d random_point() const = 0;

	//tests to see if the region intersects a given rectangle
	virtual bool intersects(const Rectangle& rectangle) const = 0;

	//determines if the line segment from start to end intersects this region
	virtual bool line_intersects(const Vector2d& start, const Vector2d& end) const = 0;

};



#endif
