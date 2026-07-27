#ifndef SURVIVALRESOURCE209845
#define SURVIVALRESOURCE209845
#include <Eigen/Dense>

/*
* An abstract class to hold a type of thing that the creture may look for
* and interact wtih. Designed to aid in polymorphism on the creature's end
* especially in plotting movement. 
*/

using Eigen::Vector2d;

class SurvivalResource
{
public:
	//get the position/anchor point of the resource
	virtual Vector2d get_position() const = 0;

	//gets the nearest point for that survival resource.
	// For a lot that would coincide with position
	virtual Vector2d get_nearest(Vector2d position) const { return get_position(); }

	//distance from the resource to the point
	double get_distance(Vector2d position) { return (get_nearest(position) - position).norm(); }

	//is the resource available for consomption?
	virtual bool is_available() const = 0;

	//what happens to the resource after somene uses it
	virtual void use() = 0;

	//updates the resource with the passage of time.
	virtual void update() = 0;

	virtual void print() const = 0;

	virtual bool is_dead() { return false; }

};

#endif
