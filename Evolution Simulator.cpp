// Evolution Simulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Eigen/Dense>
#include <random>
#include <fstream>
#include <string>
#include "Simulation.h"
#include "Rectangle.h"
#include "testingsuite.h"
#include "PopulationGenome.h"
#include <cmath>

void run_sim(double area, int iteration, PopulationGenome& pop, std::ofstream& file);

int main()
{
    assert(test_regions() && "Regions Failed");
    assert(test_resource_finder() && "Resource Finder Failed");
    assert(test_creatures() && "Creature Failed");
    assert(test_resource_set() && "Resource Set Failed");
    assert(test_desire_analyzer() && "Desire Analyzer Failed");
    assert(test_friction_analyzer() && "Frictoin Analyzer Failed");

    PopulationGenome pop{ Species::gooblet };
    std::ofstream output_file{ "behavior exploration/behavior_test.csv" };
    output_file << "map_area," << "iteration," << pop.header()  << '\n';
    
    for (int i{}; i < 8; ++i) {
        for (int iteration{}; iteration < 7; ++iteration) {
            const double area{ 1e6 + i*2e6 };
            run_sim(area, iteration, pop, output_file);
        }
    }

}

void run_sim(double area, int iteration, PopulationGenome& pop, std::ofstream& file) {
    const int food_number{ 50 };
    const int pond_number{ 2 };
    const double water_area{ 1e5 };
    const Creature::Genotype genes{ DefaultBrainGenes::gooblet() };
    const double side_length{ std::sqrt(area) };

    Simulation sim{ 0.0, side_length, 0.0, side_length, genes, 100, food_number, water_area, pond_number };
    std::cout << "area: " << area << "\titeration:"<< iteration <<'\n';
    for (int i{}; i < 7000; ++i) {
        sim.update();
        if (sim.population_size() == 0) {
            std::cout << "Population Crash!\n";
            return;
        }
        if (i % 100 == 0) {
            std::cout << i << "\t\t" << sim.population_size() <<'\n';
        }
    }
    std::cout << '\n';
    if (sim.population_size() != 0) {
        file << area << ',' << iteration << ',';
        sim.record_genome(pop, file);
        file << std::endl;
    }

}