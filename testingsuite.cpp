#include "testingsuite.h"
#include "Simulation.h"
#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <Eigen/Dense>
#include "Rectangle.h"
#include "ResourceFinder.h"
#include "WaterSource.h"

using Eigen::Vector2d;

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

bool report_dousing_attempt(SurvivalResource* pond, Vector2d position, Vector2d* correct) {
    static int attempt{};
    attempt += 1;
    if (!pond && !correct) {
        return true;
    }

    if (!pond && correct) {
        std::cout << "Failed to find water on attempt " << attempt << '\n';
        return false;
    }

    if (pond && !correct) {
        std::cout << "Found water incorrectly on attempt " << attempt << '\n';
        return false;
    }
    const Vector2d nearest_point{ pond->get_nearest(position) };
    if ((nearest_point - *correct).norm() > 1e-7) {
        std::cout << "Found incorrect water on attempt " << attempt << '\n';
        return false;
    }
    return true;
}

bool test_resource_finder() {
    std::vector<WaterSource> water{ WaterSource{10.0, 1000.0, 50.0, 1500.0},
        WaterSource{-1000, -50, -500, -20},
        WaterSource{30, 150, -300, 200},
        WaterSource{-300,-150, 40, 100}
    };
    bool answer{ true };
    ResourceFinder<WaterSource> water_manager{water, 130.0};

    Vector2d position{};
    WaterSource* pond{ water_manager.find_closest_to(Vector2d{}, 100.0)};
    Vector2d* correct = new Vector2d(10.0, 50.0);
    answer = answer || report_dousing_attempt(pond, position, correct);

    position = Vector2d(1010, 1510);
    pond = water_manager.find_closest_to(position, 100.0);
    delete correct;
    correct = new Vector2d(1000, 1500);
    answer = answer || report_dousing_attempt(pond, position, correct);

    pond = water_manager.find_closest_to(Vector2d{}, 40.0);
    delete correct;
    correct = nullptr;
    answer = answer || report_dousing_attempt(pond, position, correct);


    position = Vector2d(-40, -200);
    pond = water_manager.find_closest_to(position, 100.0);
    delete correct;
    correct = new Vector2d(-50, -200);
    answer = answer || report_dousing_attempt(pond, position, correct);
    
    delete correct;
    correct = nullptr;

    position = Vector2d{};
    std::vector<WaterSource*> ponds = water_manager.within_eyesight(position, 100.0);

    auto distanceToPoint{
        [position](const WaterSource* a, const WaterSource* b) {
            return a->get_distance(position) <= b->get_distance(position);
        }
    };

    std::sort(ponds.begin(), ponds.end(), distanceToPoint);

    constexpr int answer_size{ 3 };
    if (ponds.size() != answer_size) {
        answer = false;
        std::cout << "Found incorrect number of ponds\n";
    }
    else {
        std::array<int, answer_size> correct_ponds{ 2, 0, 1 };
        for (int i{}; i < answer_size; ++i) {
            if (correct_ponds[i] != ponds[i]->get_id()) {
                answer = false;
                std::cout << "Found incorrect pond at position " << i << '\n';
            }
        }
    }
    return answer;
}

/*
* Designed to test the capabilitie of resource set and make sure it isn't
* going too wonky
*/
bool test_resource_set() {
    bool answer{ true };
    constexpr int size{ 3 };
    std::array<Vector2d, 3> starting_positions{
        Vector2d(-5,-2), Vector2d(10.0, 10.0), Vector2d{100.0, 100.0}
    };
    ResourceSet<Creature> test_population{};
    for (int i{}; i < size; ++i) {
        test_population.insert(new Creature{ starting_positions[i], 10.0, 100.0 });
    }
    for (auto creature : test_population) {
        creature->eat();
    }

    Creature* mate = test_population.find_closest_to(Vector2d{}, 100.0);
    Vector2d* correct = new Vector2d(-5, -2);
    answer = answer || report_dousing_attempt(mate, Vector2d{}, correct);

    return answer;
}