#include "Map.h"

Environment::FoodSource::FoodSource(Vector2d position) : m_position{ position }
{
}

Environment::FoodSource::FoodSource(const Environment::FoodSource& other) : m_position{ other.get_position() }
{

}

//a fruit from the food source gets eaten
void Environment::FoodSource::used() {
	if (has_fruit()) {
		--m_fruits;
	}
}

//creates a map with food at the given locations
Environment::Environment(std::vector<Vector2d> food_positions) {
	//if there is no food this simulation will not work
	assert(!food_positions.empty());

	const int length{ static_cast<int>(food_positions.size()) };
	for (Vector2d& pos : food_positions) {
		m_food.push_back(Environment::FoodSource{ pos });
	}
}

//finds the nearest food to a given location (likely the position of a hungry creature)
//returns a null pointer if no food is available
Environment::FoodSource* Environment::nearest_food(Vector2d position){
	Environment::FoodSource* closest{};
	double current_distance{};
	for (Environment::FoodSource& bush : m_food) {
		if (!bush.has_fruit()) { continue; }
		const double distance{ (bush.get_position() - position).norm() };

		if (!closest || distance < current_distance) {
			current_distance = distance;
			closest = &bush;
		}
	}
	return closest;
}
/*

Creature* Environment::nearest_mate(Creature* creature) const{
	const Vector2d position = creature->get_position();
	Creature* closest_mate{};
	double current_distance{};
	for (Creature* potential_mate : m_population) {
		if (!(potential_mate->is_horny())) {
			continue;
		}

		const double distance{ (potential_mate->get_position() - position).norm() };
		if (!closest_mate || distance < current_distance) {
			current_distance = distance;
			closest_mate = potential_mate;
		}
	}
	return closest_mate;
}

void Environment::add_creature(Creature creature) {
	m_population.push_back(&creature);
}*/