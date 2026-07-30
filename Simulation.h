#ifndef SIMULATION_H509321
#define SIMULATION_H509321

#include "ResourceFinder.h"
#include "ResourceSet.h"
#include "Creature.h"
#include "FoodSource.h"
#include "WaterSource.h"
#include "HalfPlane.h"
#include<Eigen/Dense>

using Eigen::Vector2d;

class Simulation
{
private:
	const Rectangle m_map_base{};

	ResourceFinder<FoodSource> m_foodManager{130.0};
	ResourceFinder<WaterSource> m_waterManager{130.0};

	ResourceSet<Creature> m_femalePopulation{};
	ResourceSet<Creature> m_malePopulation{};

	//gives a sentralized place from which to generate randomness
	std::mt19937_64 m_random;

	//adds population_size male and female individuals with these speed and eyesight
	//mainly used as a helper function during set up
	void add_random_population(int population_size, double speed, double eyesight, bool bothGenders);
	//adds food_number FoodSources randomly placed
	void add_random_food(int food_number, double starting_eyesight = 100);

	//the creature in question will attempt its goal if possible
	void attempt_goal(Creature* gooblet);

	//makes sure the gooblet has a goal (if possible)
	void assign_goal(Creature* gooblet, bool isFemale);

	//updates a population of cretures
	void update_population(ResourceSet<Creature>& population, bool isFemale);

	double total_reserves(Creature::Physical_Need need) const;
public:
	Simulation(double left_bound, double right_bound, double lower_bound, double upper_bound,
		double starting_speed, double starting_eyesight, int initial_population,
		std::vector<Vector2d> foodLocations);
	Simulation(double left_bound, double right_bound, double lower_bound, double upper_bound,
		double starting_speed, double starting_eyesight, int population_size, int food_number,
		bool bothGenders = true);
	Simulation(double left_bound, double right_bound, double lower_bound, double upper_bound,
		double starting_speed, double starting_eyesight, int population_size, int food_number,
		double water_area, int pond_number,
		bool bothGenders = true);

	Simulation(Rectangle map_base,
		double starting_speed, double starting_eyesight, int population_size, int food_number,
		std::vector<WaterSource> water_sources,
		bool bothGenders = true);
	//moves the simulation forward one tick
	void update();
	//prints information about the current population
	void print_population() const;
	//prints information about the current foodsources
	void print_food() const;
	void print_water() const;

	//get some stats about the current state of the simulation
	// average speed, eyesight, and acceleration all call on average_property
	int population_size() const;
	double average_speed() const;
	double average_eyesight() const;
	double average_acceleration() const;
	double average_property(double (Creature::* property)() const) const;

	//calculates how many calories are there between all the creatures
	double total_biomass() const;
	double total_hydration() const;
};

#endif