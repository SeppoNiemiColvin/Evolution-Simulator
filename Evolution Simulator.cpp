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

int main()
{
    assert(test_regions() && "Regions Failed");
    assert(test_resource_finder() && "Resource Finder Failed");
    assert(test_creatures() && "Creature Failed");
    assert(test_resource_set() && "Resource Set Failed");
    assert(test_desire_analyzer() && "Desire Analyzer Failed");
    assert(test_friction_analyzer() && "Frictoin Analyzer Failed");

    std::string prefix{ "GoalBaesedWandering" };
    for (int i{}; i < 5; ++i) {
        wander(prefix + std::to_string(i), 300);
    }
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
