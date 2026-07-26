#include "Simulation.h"
#include <random>
#include <iostream>
#include <functional>

Simulation::Simulation(double left_bound, double right_bound, double lower_bound, double upper_bound,
	double starting_speed, double starting_eyesight, int population_size,
	std::vector<Vector2d> foodLocations) :
	m_map_base{ left_bound, right_bound, lower_bound, upper_bound },
	m_foodManager{starting_eyesight}
{
	//set up a bunch of aserts for class requirements
	//these should probably be made into exceptions as well
	assert(left_bound < right_bound && "Check Horizontal Bounds");
	assert(lower_bound < right_bound && "Check Vertical Bounds");
	assert(population_size > 0 && "Need Positive Population size");
	assert(starting_speed > 0 && "Need positive starting speed");
	assert(starting_eyesight > 0 && "Need positive starting eyesight");

	//get the food manager set up
	std::vector<FoodSource> food{};
	for (auto& location : foodLocations) {
		food.push_back(FoodSource{ location });
	}
	m_foodManager.extend(food);

	add_random_population(population_size, starting_speed, starting_eyesight, true);
}

Simulation::Simulation(double left_bound, double right_bound, double lower_bound, double upper_bound,
	double starting_speed, double starting_eyesight, int population_size, int food_number, 
	bool bothGenders) :
	m_map_base{ left_bound, right_bound, lower_bound, upper_bound },
	m_foodManager{ starting_eyesight }
{
	//set up a bunch of aserts for class requirements
	//these should probably be made into exceptions as well
	assert(left_bound < right_bound && "Check Horizontal Bounds");
	assert(lower_bound < right_bound && "Check Vertical Bounds");
	assert(population_size > 0 && "Need Positive Population size");
	assert(starting_speed > 0 && "Need positive starting speed");
	assert(starting_eyesight > 0 && "Need positive starting eyesight");

	//get the food manager set up
	std::vector<FoodSource> food{};
	for (int i{}; i < food_number; ++i) {
		food.push_back(FoodSource(m_map_base.random_point()));
	}
	m_foodManager.extend(food);

	add_random_population(population_size, starting_speed, starting_eyesight, bothGenders);
}


void Simulation::add_random_population(int population_size, double speed, double eyesight,
	bool bothGenders) {
	for (int i{}; i < population_size; ++i) {
		m_femalePopulation.insert(new Creature{ m_map_base.random_point(), speed, eyesight });
		if (bothGenders) {
			m_malePopulation.insert(new Creature{ m_map_base.random_point(), speed, eyesight });
		}
	}
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
			if (m_random() % 2 == 0) {
				m_femalePopulation.insert(gooblet->reproduce(mate));
			}
			else {
				m_malePopulation.insert(gooblet->reproduce(mate));
			}
			
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

	const Vector2d position{ gooblet->get_position() };
	const double eyesight{ gooblet->get_eyesight() };
	switch (gooblet->get_state()) {
	case Creature::State::hungry:
		gooblet->set_goal(m_foodManager.find_closest_to(position, eyesight));
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
	// We will need to first figure out the max eyesight for this
	double max_eyesight{};
	for (Creature* gooblet : m_femalePopulation) {
		if (gooblet->get_eyesight() > max_eyesight) {
			max_eyesight = gooblet->get_eyesight();
		}
	}
	for (Creature* gooblet : m_malePopulation) {
		if (gooblet->get_eyesight() > max_eyesight) {
			max_eyesight = gooblet->get_eyesight();
		}
	}
	m_foodManager.update(max_eyesight);

	//make sure populations are up to date
	update_population(m_femalePopulation, true);
	update_population(m_malePopulation, false);

}

void::Simulation::update_population(ResourceSet<Creature>& population, bool isFemale) {
	//make sure everyone has a goal
	for (Creature* animal : population) {
		assign_goal(animal, isFemale);
	}

	population.update();

	for (Creature* animal : population) {
		animal->move();
		m_map_base.pull_inside(animal);
		attempt_goal(animal);
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
	return average_property(&Creature::get_max_speed);
}

double Simulation::average_eyesight() const {
	return average_property(&Creature::get_eyesight);
}

double Simulation::average_acceleration() const {
	return average_property(&Creature::get_acceleration);
}

double Simulation::average_property(double (Creature::* property)() const)  const {
	double total{};
	for (const Creature* female : m_femalePopulation) {
		total += std::invoke(property, female);
	}

	for (const Creature* male : m_malePopulation) {
		total += std::invoke(property, male);
	}
	return total / population_size();
}