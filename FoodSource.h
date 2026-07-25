#ifndef FOODSOURCE8429
#define FOODSOURCE8429

#include<Eigen/Dense>
#include <queue>
#include "SurvivalResource.h"

using Eigen::Vector2d;

/*
* Designed to hold a source of food that replenishes over time in a given location/region
*/
class FoodSource : public SurvivalResource
{
private:
	const Vector2d m_position{};
	const int m_GROWING_TIME{ 60 };
	int m_fruits{ 4 };

	int m_timer{};
	std::queue<int> m_growing;

public:
	FoodSource(Vector2d position) : m_position{ position } {};
	FoodSource() {};

	bool is_available() const { return (m_fruits > 0); }
	Vector2d get_position() const { return m_position; }
	void use();
	void update();
	void print() const;
};

#endif

