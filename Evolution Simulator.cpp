// Evolution Simulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Eigen/Dense>
#include <unordered_set>
#include <random>
#include <vector>
#include <array>

#include "Creature.h"
#include "Environment.h"
#include "FoodSource.h"
#include "ResourceFinder.h"

//sets up where we plan to place food with local variables to conserve memory
std::array<FoodSource, 4> initialize_food() {
    std::array<Vector2d,4> foodLocations{
    Vector2d(-50.0, 75.0),
    Vector2d(300.0, 40.0),
    Vector2d(-400.0, -200.0),
    Vector2d(250.0, -100.0)
    };

    std::vector<FoodSource> food{};
    for ( const auto& position : foodLocations) {
        food.push_back(FoodSource(position));
    }
    
    std::array<FoodSource, 4> answer{};
    for (int i{}; i < 4; ++i) {
        answer[i] = FoodSource(foodLocations[i]);
    }
    return answer;
}

int main()
{
    std::mt19937_64 generator;
    std::uniform_real_distribution distribution(-500.0, 500.0);

    ResourceFinder<FoodSource,4> foodManager{ initialize_food() };

    Creature gooblet{Vector2d{}, 10.0 };
    Creature mate{ Vector2d(20.0,30.0), 10.0 };

    FoodSource* food{ foodManager.find_closest_to(gooblet.get_position()) };

    std::cout << food->get_position() << std::endl;

    if (food) {
        gooblet.move_toward(food->get_position());
    }
    gooblet.update();
    gooblet.print();
    gooblet.move_toward(food->get_position());
    gooblet.update();
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
