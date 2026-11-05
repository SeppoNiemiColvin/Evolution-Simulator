#ifndef SIMULATION_H509321
#define SIMULATION_H509321

#include "ResourceFinder.h"
#include "ResourceSet.h"
#include "Creature.h"
#include "Body.h"
#include "FoodSource.h"
#include "WaterSource.h"
#include<Eigen/Dense>
#include <fstream>
#include "PopulationGenome.h"

using Eigen::Vector2d;

struct Body::Genotype;

class Simulation
{
public:
	using Surroundings = std::unordered_map<Ecology, std::vector<MovementAnalyzer::Observation>>;
private:

	const Rectangle m_map_base{};

	ResourceFinder<FoodSource> m_foodManager{130.0};
	ResourceFinder<WaterSource> m_waterManager{130.0};

	ResourceSet<Creature> m_femalePopulation{};
	ResourceSet<Creature> m_malePopulation{};
	PopulationGenome m_genome;

	//gives a sentralized place from which to generate randomness
	std::mt19937_64 m_random;

	//adds population_size male and female individuals with these speed and eyesight
	//mainly used as a helper function during set up
	void add_random_population(int population_size, const Creature::Genotype& genes, bool bothGenders);
	//adds food_number FoodSources randomly placed
	void add_random_food(int food_number, double starting_eyesight = 100);

	//the creature in question will attempt its goal if possible
	void attempt_goal(Creature* gooblet, bool isFemale);


	//updates a population of cretures
	void update_population(ResourceSet<Creature>& population, bool isFemale);

	double total_reserves(Body::Need need) const;

	//compiles the features within eyesight of animal 
	Surroundings look_around(Creature* animal, bool isFemale);
public:
	Simulation(double left_bound, double right_bound, double lower_bound, double upper_bound,
		const Creature::Genotype& genes, int initial_population,
		std::vector<Vector2d>& foodLocations);
	Simulation(double left_bound, double right_bound, double lower_bound, double upper_bound,
		const Creature::Genotype& genes,  int population_size, int food_number,
		bool bothGenders = true);
	Simulation(double left_bound, double right_bound, double lower_bound, double upper_bound,
		const Creature::Genotype& genes,  int population_size, int food_number,
		double water_area, int pond_number,
		bool bothGenders = true);

	Simulation(Rectangle map_base,
		const Creature::Genotype& genes,  int population_size, int food_number,
		std::vector<WaterSource>& water_sources,
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

	//runs the simulation forward for bound intervals and records the average population properties over that time
	void run_and_record(const std::string& file_name, int bound, int sample_frequency);
	//records the genome to a PopulationGenome object
	void fill_population_genome();
	//gets the header of the genome (likely to be printed to a file)
	std::string get_genome_header() { return m_genome.header(); }
	//makes sure the population genome is up to date and returns it
	PopulationGenome get_genome();
	//records the position of all creatures
	void record_population_positions(std::ofstream& output_file, int time) const;
	//record the location of all the food items
	void record_food(std::ofstream& output_file) const;
	//record the location of all ponds
	void record_water(std::ofstream& output_file) const;
};

#endif