// Evolution Simulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Eigen/Dense>
#include <unordered_set>
#include <random>
#include <vector>
#include <array>

#include "Simulation.h"
#include "Rectangle.h"

int main()
{
    double map_size{ 2e3 };
    double food_radius{ 30.0 };
    double food_area{ food_radius * food_radius * 3.14159 };
    int food_number{ static_cast<int>(map_size * map_size / food_area) };
    std::cout << food_number << std::endl;

    Simulation simulation{ -map_size, map_size, -map_size, map_size, 10.0, 10 * food_radius, 20,food_number };

    for (int time{}; time <= 1e4; ++time) {
        if (time % 100 == 0) {
            std::cout << "Time: " << time << std::endl;
            std::cout << "Population: " << simulation.population_size() << '\n';
            std::cout << "Average Speed: " << simulation.average_speed() << '\n';
            std::cout << "Averge Acceleration: " << simulation.average_acceleration() << '\n';
            std::cout << "Average Eyesight: " << simulation.average_eyesight() << '\n';
            std::cout << std::endl;
        }
        simulation.update();
    }


    Rectangle base_map{ 0.0, 50.0, 0.0 , 50.0 };
    Rectangle obstacle{ 20.0, 30.0, 20.0, 35.0 };
    Creature* gooblet = new Creature(Vector2d(15.0, 15.0), 10.0, 50.0);

    return 0;


}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
