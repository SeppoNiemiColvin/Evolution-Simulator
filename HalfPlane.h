#ifndef HALF_PLANE_H71023
#define HALF_PLANE_H71023

#include "Region.h"
#include <Eigen/Dense>
class Creature;

using Eigen::Vector2d;

/*
* A region that models a half plane in point normal form.
* Mainly designed to act as a wall, but probably phased out in favor of rectangles
* pulling creatures in
*/
class HalfPlane : public Region
{
private:
	//note in construction we will make sure m_normal is a unit vector
	Vector2d m_normal{ 1,0 }; 
	double m_affine{};
public:
	HalfPlane(Vector2d normal, double affine_djusment);
	HalfPlane(Vector2d normal, Vector2d point);
	HalfPlane();

	bool is_inside(const Vector2d& position) const;
	Vector2d get_nearest(const Vector2d& position) const;
	Vector2d point_on_line() const;
	double get_distance(const Vector2d& position) const;
	void push_out(Creature* creature) const;

	//this will produce the point from point_on_line
	//mainly included for consistency with other Region classes
	Vector2d random_point() const { return point_on_line(); }
};

#endif
