#include "Simulation.h"
#include <random>
#include <iostream>

Simulation::Simulation(double left_bound, double right_bound, double lower_bound, double upper_bound,
	double starting_speed, double starting_eyesight, int population_size,
	std::vector<Vector2d> foodLocations) :
	m_left_bound{left_bound},
	m_right_bound{right_bound},
	m_lower_bound{lower_bound},
	m_upper_bound{upper_bound}
{
	//set up a bunch of aserts for class requirements
	//these should probably be made into exceptions as well
	assert(m_left_bound < m_right_bound && "Check Horizontal Bounds");
	assert(m_lower_bound < m_right_bound && "Check Vertical Bounds");
	assert(population_size > 0 && "Need Positive Population size");
	assert(starting_speed > 0 && "Need positive starting speed");
	assert(starting_eyesight > 0 && "Need positive starting eyesight");
	
	//get the food manager set up
	std::vector<FoodSource> food{};
	for (auto& location : foodLocations) {
		food.push_back(FoodSource{ location });
	}
	m_foodManager.extend(food);

	add_random_population(population_size, starting_speed, starting_eyesight);
}

Simulation::Simulation(double left_bound, double right_bound, double lower_bound, double upper_bound,
	double starting_speed, double starting_eyesight, int population_size, int food_number) :
	m_left_bound{ left_bound },
	m_right_bound{ right_bound },
	m_lower_bound{ lower_bound },
	m_upper_bound{ upper_bound }
{
	//set up a bunch of aserts for class requirements
	//these should probably be made into exceptions as well
	assert(m_left_bound < m_right_bound && "Check Horizontal Bounds");
	assert(m_lower_bound < m_right_bound && "Check Vertical Bounds");
	assert(population_size > 0 && "Need Positive Population size");
	assert(starting_speed > 0 && "Need positive starting speed");
	assert(starting_eyesight > 0 && "Need positive starting eyesight");

	//get the food manager set up
	std::vector<FoodSource> food{};
	for (int i{}; i < food_number; ++i) {
		food.push_back(FoodSource(random_position()));
	}
	m_foodManager.extend(food);

	add_random_population(population_size, starting_speed, starting_eyesight);
}

Vector2d Simulation::random_position() {
	//set up our random distributions
	static std::uniform_real_distribution horizontal(m_left_bound, m_right_bound);
	static std::uniform_real_distribution vertical(m_lower_bound, m_right_bound);

	return Vector2d(horizontal(m_random), vertical(m_random));
}

void Simulation::add_random_population(int population_size, double speed, double eyesight) {
	for (int i{}; i < population_size; ++i) {
		m_femalePopulation.insert(new Creature{ random_position(), speed, eyesight });
		m_malePopulation.insert(new Creature{ random_position(), speed, eyesight });
	}
}

void Simulation::males_eat() {
	for (auto& male : m_malePopulation) {
		male->eat();
	}

	m_malePopulation.update();
}

void Simulation::attempt_goal(Creature* gooblet) {
	if (gooblet->goal_in_reach()) {
		switch (gooblet->get_state()) {
		case Creature::State::hungry:
			//eat
			gooblet->get_goal()->use();
			gooblet->eat();
			break;
		case Creature::State::horny:
			//reproduce
			Creature* mate{ m_malePopulation.find_closest_to(gooblet->get_position()) };
			assert(mate && "goal_in_reach should ensure gooblet-> get_goal() is an option");
			gooblet->get_goal()->use();
			gooblet->use();
			
			//currently adding to female
			//but should in a random desicion to put in male or female
			m_femalePopulation.insert(gooblet->reproduce(mate));
			break;
		}
		gooblet->set_goal(nullptr);
	}
}

void Simulation::assign_goal(Creature* gooblet, bool isFemale) {
	//only assign a goal if it is seaking one
	if (!(gooblet->is_seeking())) {
		return;
	}

	Vector2d position = gooblet->get_position();
	switch (gooblet->get_state()) {
	case Creature::State::hungry:
		gooblet->set_goal(m_foodManager.find_closest_to(position));
		break;
	case Creature::State::horny:
		if (isFemale) {
			gooblet->set_goal(m_malePopulation.find_closest_to(position));
		}
		else {
			gooblet->set_goal(m_femalePopulation.find_closest_to(position));
		}
		break;
	}
}

void Simulation::update() {
	//make sure food is up to date
	m_foodManager.update();

	//make sure everyone has a goal
	for (Creature* female : m_femalePopulation) {
		assign_goal(female, true);
	}

	m_femalePopulation.update();

	for (Creature* female : m_femalePopulation) {
		female->move();
		attempt_goal(female);
	}

}

void Simulation::print_population() const {
	std::cout << "Females: " << std::endl;
	for (Creature* female : m_femalePopulation) {
		female->print();
	}
	std::cout << std::endl;

	std::cout << "Males: " << std::endl;
	for (Creature* male : m_malePopulation) {
		male->print();
	}
	std::cout << std::endl;
}

void Simulation::print_food() const {
	m_foodManager.print();
}

int Simulation::population_size() const {
	return m_malePopulation.size() + m_femalePopulation.size();
}

double Simulation::average_speed() const {
	double total{};
	for (const Creature* female : m_femalePopulation) {
		total += female->get_speed();
	}

	for (const Creature* male : m_malePopulation) {
		total += male->get_speed();
	}
	return total / population_size();
}