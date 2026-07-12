#ifndef SURVIVALRESOURCE209845
#define SURVIVALRESOURCE209845
#include <Eigen/Dense>

using Eigen::Vector2d;

class SurvivalResource
{
public:
	//get the position/anchor point of the resource
	virtual Vector2d get_position() const = 0;

	//for a lot of resources there aren't muliple points to choose from
	virtual Vector2d get_nearest(Vector2d position) const { return get_position(); }

	//distance from the resource to the point
	double get_distance(Vector2d position) { return (get_nearest(position) - position).norm(); }

	//is the resource available for consomption?
	virtual bool is_available() const = 0;

	//what happens to the resource after somene uses it
	virtual void use() = 0;

	//updates the resource with the passage of time.
	virtual void update() = 0;
};

#endif
