#ifndef MAP2385712
#define MAP2385712

#include <vector>
#include <Eigen/Dense>
#include <unordered_set>

using Eigen::Vector2d;

class Creature;

class Environment
{
public:
	class FoodSource
	{
	private:
		const Vector2d m_position{};
		int m_fruits{ 4 };

	public:
		FoodSource(Vector2d position);
		FoodSource(const FoodSource& other);

		bool has_fruit() const { return (m_fruits > 0); } 
		Vector2d get_position() const { return m_position; }
		void used();
	};

private:
	//models where food resources are
	std::vector<FoodSource> m_food{};
	std::unordered_set<Creature*> m_population{}; 

public:
	Environment(std::vector<Vector2d> food_positions);

	FoodSource* nearest_food(Vector2d position);
	Creature* nearest_mate(Creature* creature) const;
	void add_creature(Creature* creature);
	void step_forward();
};

#endif
