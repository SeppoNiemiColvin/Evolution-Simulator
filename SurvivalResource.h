#ifndef SURVIVALRESOURCE209845
#define SURVIVALRESOURCE209845
#include <Eigen/Dense>

using Eigen::Vector2d;

class SurvivalResource
{
public:
	virtual Vector2d get_position() const = 0;
	virtual bool is_available() const = 0;
	virtual void use() = 0;
	virtual void update() = 0;
};

#endif
