// Evolution Simulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Creature.h"
#include "Map.h"
#include <Eigen/Dense>
#include <unordered_set>
#include <random>
#include <vector>

int main()
{
    std::mt19937_64 generator;
    std::uniform_real_distribution distribution(-500.0, 500.0);

    std::vector<Vector2d> foodLocations{
        Vector2d(-50.0, 75.0),
        Vector2d(300.0, 40.0),
        Vector2d(-400.0, -200.0),
        Vector2d(250.0, -100.0)
    };

    Environment map{ foodLocations };

    Creature gooblet{ Vector2d{}, 10.0 };
    Environment::FoodSource* food{ map.nearest_food(gooblet.get_position()) };

    std::cout << food->get_position() << std::endl;

    if (food) {
        gooblet.move_toward(food->get_position());
    }
    gooblet.update_state();
    gooblet.print();
    gooblet.move_toward(food->get_position());
    gooblet.update_state();
    gooblet.print();

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
