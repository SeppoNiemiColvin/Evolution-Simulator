#include "Simulation.h"
#include <random>
#include <cmath>
#include <iostream>
#include <functional>
#include "ReproHelpers.h"
#include "PopulationGenome.h"

Simulation::Simulation(double left_bound, double right_bound, double lower_bound, double upper_bound,
	const Creature::Genotype& genes, int population_size,
	std::vector<Vector2d>& foodLocations) :
	m_map_base{ left_bound, right_bound, lower_bound, upper_bound },
	m_foodManager{genes.eyesight + 30.0}
{
	//set up a bunch of aserts for class requirements
	//these should probably be made into exceptions as well
	assert(left_bound < right_bound && "Check Horizontal Bounds");
	assert(lower_bound < right_bound && "Check Vertical Bounds");
	assert(population_size > 0 && "Need Positive Population size");
	genes.test_validity();

	//get the food manager set up
	std::vector<FoodSource> food{};
	for (auto& location : foodLocations) {
		food.push_back(FoodSource{ location });
	}
	m_foodManager.extend(food);

	add_random_population(population_size, genes, true);
}

Simulation::Simulation(double left_bound, double right_bound, double lower_bound, double upper_bound,
	const Creature::Genotype& genes, int population_size, int food_number,
	bool bothGenders) :
	m_map_base{ left_bound, right_bound, lower_bound, upper_bound },
	m_foodManager{ genes.eyesight + 30.0}
{
	//set up a bunch of aserts for class requirements
	//these should probably be made into exceptions as well
	assert(left_bound < right_bound && "Check Horizontal Bounds");
	assert(lower_bound < right_bound && "Check Vertical Bounds");
	assert(population_size > 0 && "Need Positive Population size");
	genes.test_validity();

	//get the food manager set up
	std::vector<FoodSource> food{};
	for (int i{}; i < food_number; ++i) {
		food.push_back(FoodSource(m_map_base.random_point()));
	}
	m_foodManager.extend(food);

	add_random_population(population_size, genes, bothGenders);
}

//still in progress
Simulation::Simulation(Rectangle map_base,
	const Creature::Genotype& genes,  int population_size, int food_number,
	std::vector<WaterSource>& water_sources,
	bool bothGenders) :
	m_map_base{ map_base },
	m_foodManager{ genes.eyesight + 30.0 },
	m_waterManager{ water_sources, genes.eyesight + 30.0}
{
	//set up a bunch of aserts for class requirements
	//these should probably be made into exceptions as well
	assert(population_size > 0 && "Need Positive Population size");
	genes.test_validity();

	add_random_food(food_number, genes.eyesight);

	add_random_population(population_size, genes, bothGenders);

}


Simulation::Simulation(double left_bound, double right_bound, double lower_bound, double upper_bound,
	const Creature::Genotype& genes, int population_size, int food_number,
	double water_area, int pond_number,
	bool bothGenders) :
	m_map_base{ left_bound, right_bound, lower_bound, upper_bound },
	m_foodManager{ genes.eyesight + 30.0 }
{
	double starting_speed{ genes.body_genes.max_speed };
	//set up a bunch of aserts for class requirements
	//these should probably be made into exceptions as well
	assert(left_bound < right_bound && "Check Horizontal Bounds");
	assert(lower_bound < right_bound && "Check Vertical Bounds");
	assert(population_size > 0 && "Need Positive Population size");
	genes.test_validity();
	assert(m_map_base.get_area() > water_area && "No land for the creatures");
	assert((water_area > pond_number * starting_speed * starting_speed * 4.5) && "Not enough water for the ponds");


	// create some non_overlapping watersources
	std::vector<WaterSource> water{};
	double remaining_area{ water_area };
	for (int i{}; i < pond_number -1; ++i) {
		const double max_dimension{ std::sqrt(remaining_area - (pond_number- i -1) * 4.001* starting_speed * starting_speed) };
		std::uniform_real_distribution distribution(2*starting_speed, max_dimension);
		double width{ distribution(m_random) };
		double height{ distribution(m_random) };
		remaining_area -= width * height;

		// suptracting starting_speed from each so it won't hang off the edge too much
		Rectangle pond{ -starting_speed, width - starting_speed, -starting_speed, height - starting_speed };
		//find a place we can put the new pond
		Vector2d point{ m_map_base.random_point()};
		while (overlap(water, pond + point)) {
			point = m_map_base.random_point();
		}

		water.push_back(WaterSource(pond + point));
	}

	//need the last pond to take up the remaiing area
	std::uniform_real_distribution distribution(2 * starting_speed, remaining_area / (2 * starting_speed));
	double width{ distribution(m_random) };
	double height{ remaining_area / width };
	Rectangle pond{ -starting_speed, width -starting_speed, -starting_speed, height - starting_speed };
	Vector2d point{ m_map_base.random_point() };
	while (overlap(water, pond + point)) {
		point = m_map_base.random_point();
	}
	water.push_back(WaterSource(pond + point));
	m_waterManager.extend(water);

	//get the food manager set up
	add_random_food(food_number, genes.eyesight);

	add_random_population(population_size, genes, bothGenders);
}

void Simulation::add_random_food(int food_number, double starting_eyesight) {
	//get the food manager set up
	std::vector<FoodSource> food{};
	for (int i{}; i < food_number; ++i) {
		//find a point at least a constant times starting_eyesight away from any water sources
		//to force the creatures to wander around.
		Vector2d point{ m_map_base.random_point() };
		while (m_waterManager.find_closest_to(point, starting_eyesight * 0.7)) {
			point = m_map_base.random_point();
		}

		food.push_back(FoodSource(point));
	}
	m_foodManager.extend(food);

}
void Simulation::add_random_population(int population_size, const Creature::Genotype& genes,
	bool bothGenders) {
	for (int i{}; i < population_size; ++i) {
		Vector2d position{ m_map_base.random_point() };
		m_femalePopulation.insert(new Creature{ position, genes });
	}
	//handle males now
	if (!bothGenders) {
		return;
	}
	for (int i{}; i < population_size; ++i) {
		Vector2d position{ m_map_base.random_point() };
		m_malePopulation.insert(new Creature{position, genes});
	}
}



void Simulation::attempt_goal(Creature* gooblet, bool isFemale) {
	if (gooblet->goal_in_reach()) {
		Creature::Goal& goal{ gooblet->get_goal() };
		switch (goal.type) {
		case MetaEcology::water:
			gooblet->drink();
			break;
		case MetaEcology::food:
			//eat
			gooblet->eat();
			break;
		case MetaEcology::mate:
			Creature* mate{ dynamic_cast<Creature*>(goal.desire.object)};

			assert(mate && "goal_in_reach should ensure there is a mate in reach");
			gooblet->use();
			//currently adding to female
			//but should in a random desicion to put in male or female
			Creature* baby{ gooblet->reproduce(mate) };
			if (coinFlip()) {
				m_femalePopulation.insert(baby);
			}
			else {
				m_malePopulation.insert(baby);
			}
			
			break;
		}
		goal.desire.object->use();
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
	m_waterManager.update(max_eyesight);
	m_femalePopulation.update(max_eyesight);
	//having the males try their goals now so that way the female's positions are still fresh
	for (Creature* male : m_malePopulation) {
		attempt_goal(male, false);
	}

	m_malePopulation.update(max_eyesight);
	for (Creature* female : m_femalePopulation) {
		attempt_goal(female, true);
	}
	//make sure populations are up to date
	update_population(m_femalePopulation, true);
	update_population(m_malePopulation, false);

}

Simulation::Surroundings Simulation::look_around(Creature* animal, bool isFemale) {
	//ask animal what types of things
	std::vector<Ecology> flags{ animal->get_flags() };
	const Vector2d position{ animal->get_position() };
	const double eyesight{ animal->get_eyesight() };
	Surroundings things_around{};
	for (auto flag : flags) {
		//pull nearby things of that desire
		std::vector<MovementAnalyzer::Observation> nearby_resources;
		std::vector<FoodSource*> bushes{};
		switch (flag) {
		case Ecology::bush:
			bushes = m_foodManager.within_eyesight(position, eyesight);
			nearby_resources.resize(bushes.size());
			for (int index{}; index < bushes.size(); ++index) {
				nearby_resources[index] = MovementAnalyzer::Observation{ bushes[index] };
				nearby_resources[index].from_perspective(position);
				nearby_resources[index].intensity = bushes[index]->get_fruit();
			}
			break;
		case Ecology::pond:
			nearby_resources = m_waterManager.look_around(position, eyesight);
			break;

		case Ecology::gooblet_mate:
			if (!(animal->is_available())) {
				break;
			}
			if (isFemale) {
				nearby_resources = m_malePopulation.look_around(position, eyesight);
			}
			else {
				nearby_resources = m_femalePopulation.look_around(position, eyesight);
			}
			break;
		}
		if (!nearby_resources.empty()) {
			//remove any blocked by ponds
			things_around[flag] = nearby_resources;
		}
	}
	return things_around;
}

void Simulation::update_population(ResourceSet<Creature>& population, bool isFemale) {
	for (Creature* animal : population) {
		Surroundings things_around{ look_around(animal, isFemale) };
		animal->move(things_around);

		//implement obstacles
		m_map_base.pull_inside(animal);
		WaterSource* closest_pond{ m_waterManager.find_closest_to(
			animal->get_position(), 1.0) };
		if (closest_pond) {
			closest_pond->push_out(animal);
		}

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

void Simulation::print_water() const {
	m_waterManager.print();
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
	return average_property(&Creature::get_max_acceleration);
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

double Simulation::total_reserves(Body::Need need) const {
	double total{};
	for (const Creature* female : m_femalePopulation) {
		total += female->get_current_reserve(need);
	}
	for (const Creature* male : m_malePopulation) {
		total += male->get_current_reserve(need);
	}
	return total;
}

double Simulation::total_biomass() const {
	return total_reserves(Body::Need::calories);
}

double Simulation::total_hydration() const {
	return total_reserves(Body::Need::water);
}

void Simulation::run_and_record(const std::string& file_name, int bound) {
	std::ofstream output_file{ file_name  +".csv"};
	if (!output_file) {
		std::cerr << "Unable to open output file\n";
		return;
	}

	//header for the csv
	PopulationGenome pop{ Species::gooblet };
	output_file << pop.header() << ",Biomass" << '\n';

	//run the simulationa nd record results. print occasionally to keep updated
	for (int time{}; time < bound; ++time) {
		pop.clear();
		for (auto animal : m_femalePopulation) {
			pop += animal;
		}
		for (auto animal : m_malePopulation) {
			pop += animal;
		}
		const double biomass{ total_biomass() };
		output_file << pop << ',' << biomass <<'\n';

		if (pop.get_population_size() == 0) {
			break;
		}

		update();

		if (time % 100 == 0) {

			std::cout << "Time:\t" << time << '\n';
			std::cout << "Population:\t" << pop.get_population_size() << '\n';
			std::cout << "Biomass: \t" << biomass << '\n';
			Creature::Genotype average{ pop.average_genotype() };
			std::cout << "Avg. Speed:\t" << average.body_genes.max_speed << '\n';
			std::cout << "Avg. Acc:\t" << average.body_genes.max_acceleration << '\n';
			std::cout << "Avg. Eyesight:\t" << average.eyesight << '\n';
			std::cout << std::endl;

		}
	}
}

void Simulation::record_population_positions(std::ofstream& output_file, int time) const {
	for (auto female : m_femalePopulation) {
		Vector2d data{ female->get_position() };
		output_file << data(0) << ',' << data(1) << ',';
		data = female->get_velocity();
		output_file << data(0) << ',' << data(1) << ',';
		output_file << time << '\n';
	}
	for (auto male : m_malePopulation) {
		Vector2d data{ male->get_position() };
		output_file << data(0) << ',' << data(1) << ',';
		data = male->get_velocity();
		output_file << data(0) << ',' << data(1) << ',';
		output_file << time << '\n';
	}
}

void Simulation::record_food(std::ofstream& output_file) const {
	output_file << "x,y\n";
	
	auto end{ m_foodManager.cend() };
	for (auto start{ m_foodManager.cbegin() }; start != end; ++start) {
		const Vector2d position{ start->get_position() };
		output_file << position(0) << ',' << position(1) << '\n';
	}
}

void Simulation::record_water(std::ofstream& output_file) const {
	output_file << "left,right,lower,upper\n";

	auto end{ m_waterManager.cend() };
	for (auto start{ m_waterManager.cbegin() }; start != end; ++start) {
		output_file << start->get_left() << ',';
		output_file << start->get_right() << ',';
		output_file << start->get_lower() << ',';
		output_file << start->get_upper() << '\n';
	}
}