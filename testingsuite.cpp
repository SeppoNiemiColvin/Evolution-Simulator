#include "testingsuite.h"
#include "Simulation.h"
#include <iostream>
#include <fstream>


/*
* Sees how long a single creature can survive on its own up to some bound.
* I.e. if the creature survives until bound units, we'll stop and just return
* bound, but if the creature dies earlier we return the time when that happened.
*/
int test_survivability(int bound) {
	Simulation simulation{ 0.0, 250.0, 0.0, 250.0, 10.0, 100.0, 1, 1 , false };
	for (int time{}; time < bound; ++time) {
		simulation.update();

		if (simulation.population_size() == 0) {
			return time;
		}
	}

	return bound;
}

void wander(int bound) {
	Simulation simulation{ 0.0, 250.0, 0.0, 250.0, 10.0, 100.0, 1, 1, false };
	for (int time{}; time < bound; ++time) {
		simulation.update();
		std::cout << "Time: " << time << '\n';
        simulation.print_population();
		std::cout << '\n' << '\n';
		if (simulation.population_size() == 0) {
			return;
		}
	}
}

//note that sim1 was map_size 2e3 and food_diameter 30.0
void test_density(double map_size, double food_diameter, std::string& file_path) {
	const double food_area{ food_diameter * food_diameter * 3.14159 };
	const int food_number{ static_cast<int>(map_size * map_size / food_area) };
	std::cout << "Available Food: "<< food_number << std::endl;

    Simulation simulation{ -map_size, map_size, -map_size, map_size,
        10.0, 100.0, 20, food_number };


    //set up an output file to receive the simulation data
    std::ofstream output_file{ file_path };
    if (!output_file) {
        std::cerr << "Unable to open output file\n";
        return;
    }

    //header for the csv
    output_file << "Population" << ",Speed" << ",Acceleration" << ",Eyesight" << '\n';

    //run the simulationa nd record results. print occasionally to keep updated
    for (int time{}; time < 1e4; ++time) {
        const int population{ simulation.population_size() };
        const double speed{ simulation.average_speed() };
        const double acceleration{ simulation.average_acceleration() };
        const double eyesight{ simulation.average_eyesight() };

        output_file << population << ',' << speed << ',' << acceleration << ',' << eyesight << '\n';
        simulation.update();

        if (time % 100 == 0) {
            std::cout << "Time:\t" << time << '\n';
            std::cout << "Population:\t" << population << '\n';
            std::cout << "Avg. Speed:\t" << speed << '\n';
            std::cout << "Avg. Acc:\t" << acceleration << '\n';
            std::cout << "Avg. Eyesight:\t" << eyesight << '\n';
            std::cout << std::endl;

        }
    }
}