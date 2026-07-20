#ifndef HALF_PLANE_H71023
#define HALF_PLANE_H71023

#include "Region.h"
#include <Eigen/Dense>


using Eigen::Vector2d;

class HalfPlane : public Region
{
private:
	//note in construction we will make sure m_normal is a unit vector
	Vector2d m_normal{ 1,0 }; 
	double m_affine{};
public:
	HalfPlane(Vector2d normal, double affine_djusment);
	HalfPlane(Vector2d normal, Vector2d point);
	bool is_inside(const Vector2d position) const;
	Vector2d get_nearest(const Vector2d position) const;
	Vector2d point_on_line() const;
	double get_distance(const Vector2d position) const;
	void push_out(Creature* creature) const;
};

#endif
