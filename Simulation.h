#ifndef SIMULATION_H509321
#define SIMULATION_H509321

#include "ResourceFinder.h"
#include "ResourceSet.h"
#include "Creature.h"
#include "FoodSource.h"
#include<Eigen/Dense>

using Eigen::Vector2d;

class Simulation
{
private:
	const double m_left_bound{};
	const double m_right_bound{};
	const double m_upper_bound{};
	const double m_lower_bound{};

	ResourceFinder<FoodSource> m_foodManager{};

	ResourceSet<Creature> m_femalePopulation{};
	ResourceSet<Creature> m_malePopulation{};

	//gives a sentralized place from which to generate randomness
	std::mt19937_64 m_random;

	//gets a random position within the bounds for this simulation
	Vector2d random_position();
	//adds population_size male and female individuals with these speed and eyesight
	//mainly used as a helper function during set up
	void add_random_population(int population_size, double speed, double eyesight);
	//adds food_number FoodSources randomly placed
	void add_random_food(int food_number);

	//the creature in question will attempt its goal if possible
	void attempt_goal(Creature* gooblet);

	//makes sure the gooblet has a goal (if possible)
	void assign_goal(Creature* gooblet, bool isFemale);
public:
	Simulation(double left_bound, double right_bound, double lower_bound, double upper_bound,
		double starting_speed, double starting_eyesight, int initial_population,
		std::vector<Vector2d> foodLocations);
	Simulation(double left_bound, double right_bound, double lower_bound, double upper_bound,
		double starting_speed, double starting_eyesight, int population_size, int food_number);

	//moves the simulation forward one tick
	void update();

	//prints information about the current population
	void print_population() const;
	//prints information about the current foodsources
	void print_food() const;

	//helper member function so that the males are all immediately sexually available
	//helpful during initial testing when I want only one gender walking around
	void males_eat();
};

#endif