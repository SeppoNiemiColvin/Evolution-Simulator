#include "testingsuite.h"
#include "Simulation.h"
#include <iostream>
#include <fstream>
#include <array>
#include "Rectangle.h"

/*
* Sees how long a single creature can survive on its own up to some bound.
* I.e. if the creature survives until bound units, we'll stop and just return
* bound, but if the creature dies earlier we return the time when that happened.
*/
int test_survivability(int bound, double food_distance, double water_density) {
    const double water_area{ 2.5e5 * water_density };
    const int pond_number{ static_cast<int>(water_area / 1000) };
    const int food_number{ static_cast<int>((500 * 500 - water_area) * 4.0 /
        (food_distance * food_distance * 3.14159)) };
	Simulation simulation{ 0.0, 500.0, 0.0, 500.0, 10.0, 100.0, 1,food_number , water_area, pond_number, false };
	for (int time{}; time < bound; ++time) {
		simulation.update();

		if (simulation.population_size() == 0) {
			return time;
		}
	}

	return bound;
}

void wander(int bound) {
	Simulation simulation{ 0.0, 250.0, 0.0, 250.0, 10.0, 100.0, 1, 60,  2e4, 4,false };
    simulation.print_water();
    simulation.print_population();

	for (int time{}; time < bound; ++time) {
		simulation.update();
		std::cout << "Time: " << time << '\n';
        simulation.print_population();
        simulation.print_water();

		std::cout << '\n' << '\n';
		if (simulation.population_size() == 0) {
			return;
		}
	}
}

//note that sim1 was map_size 2e3 and food_diameter 30.0
void test_density(double map_size, double food_diameter, std::string& file_path) {
    const double water_area{ map_size * map_size / 15 };
    const int pond_number{ static_cast<int>(water_area / 1e3) };

	const double food_area{ food_diameter * food_diameter * 3.14159 };
	const int food_number{ static_cast<int>((map_size * map_size - water_area)/ food_area) };
	std::cout << "Available Food: "<< food_number << std::endl;

    Simulation simulation{ -map_size, map_size, -map_size, map_size,
        10.0, 100.0, 100, food_number, water_area, pond_number };


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

bool test_regions() {
    constexpr int size{ 7 };

    const std::array<Rectangle, size> test_rects{
        Rectangle{0.0, 100.0, 50.0, 200.0},
        Rectangle{50.0, 300.0, 150.0, 250.0},
        Rectangle{60.0, 110.0, 0.0, 100.0 },
        Rectangle{-120.0, 20.0, 120.0, 230.0},
        Rectangle{-140.0, 10.0, 20.0, 75.0},
        Rectangle{-20.0, 400.0, 0.0, 300.0},
        Rectangle{300.0, 500.0, 240.0, 600.0}
    };
    constexpr std::array<double, size> areas{
        100.0 * 150.0,
        250.0 * 100.0,
        50.0 * 100.0,
        140.0 * 110.0,
        150.0 * 55.0,
        420.0 * 300.0,
        200.0 * 360.0
    };

    constexpr std::array<bool, size> intersections{
        true, true, true, true, true, true, false
    };

    const Vector2d start(-20.0, -10.0);
    const Vector2d end(110.0, 110.0);

    constexpr std::array<bool, size> line_intersect{
        true, false, true, false,false, true, false
    };
    for (int i{}; i < size; ++i) {
        if (test_rects[i].get_area() != areas[i]) {
            std::cout << "Area failed at " << i << '\n';
            return false;
        }
        if (test_rects[0].intersects(test_rects[i]) != intersections[i]) {
            std::cout << "Intersection failed at " << i << '\n';
            return false;
        }
        
        if (test_rects[i].line_intersects(start, end) != line_intersect[i]) {
            std::cout << "Line Intersection failed at " << i << '\n';
            return false;
        }
    }


    return true;
}

bool test_resource_finder() {
    Creature* gooblet = new Creature{ Vector2d{},10.0, 100.0 };
    return true;
}