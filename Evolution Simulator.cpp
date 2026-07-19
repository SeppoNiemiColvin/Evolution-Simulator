// Evolution Simulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Eigen/Dense>
#include <unordered_set>
#include <random>
#include <vector>
#include <array>

#include "Simulation.h"


void acheive_goal(Creature& gooblet);

int main()
{
    std::vector<Vector2d> foodLocations{
        Vector2d(-50.0, 75.0), //0
        Vector2d(300.0, 40.0), //1
        Vector2d(-400.0, -200.0), //2
        Vector2d(250.0, -100.0), //3
        Vector2d(100.0, 70.0), //4
        Vector2d(-200, 400.0),//5
        Vector2d(20.0, -300.0), //6
        Vector2d(-130.0, -350.0), //7
        Vector2d(-450.0, 150.0), //8
        Vector2d(400.0, 350.0), //9
        Vector2d(10.0, 200.0) //10
    };
    double map_size{ 1e3 };
    double food_radius{ 30.0 };
    double food_area{ food_radius * food_radius * 3.14159 };
    int food_number{ static_cast<int>(map_size * map_size / food_area) };
    std::cout << food_number << std::endl;
    Simulation simulation{ -map_size, map_size, -map_size, map_size, 10.0, 4 * food_radius, 20,food_number };

    for (int time{}; time < 7000; ++time) {
        if (time % 100 == 0) {
            std::cout << "Time: " << time << std::endl;
            std::cout << "Population: " << simulation.population_size() << std::endl;
            std::cout << "Average Speed: " << simulation.average_speed() << std::endl;
            std::cout << "Average Eyesight: " << simulation.average_eyesight() << std::endl;
            std::cout << std::endl;
        }
        simulation.update();
    }

    std::cout << std::endl << simulation.population_size() << std::endl;
    std::cout << simulation.average_speed() << std::endl;
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
