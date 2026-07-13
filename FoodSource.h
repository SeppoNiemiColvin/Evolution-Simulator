#ifndef FOODSOURCE8429
#define FOODSOURCE8429

#include<Eigen/Dense>
#include "SurvivalResource.h"

using Eigen::Vector2d;

class FoodSource : public SurvivalResource
{
private:
	const Vector2d m_position{};
	int m_fruits{ 4 };

public:
	FoodSource(Vector2d position) : m_position{ position } {};
	FoodSource() {};

	bool is_available() const { return (m_fruits > 0); }
	Vector2d get_position() const { return m_position; }
	void use() { --m_fruits; }
	void update();
	void print() const;
};

#endif

