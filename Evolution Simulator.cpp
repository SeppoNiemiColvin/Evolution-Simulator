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
    //set up map sizes and food density to have multiple simulations at that density
    double food_radius{ 30.0};
    double map_delta{ 2.5e2 };
    double base_map_size{ 1e3 };

    for (int i{ 0 }; i < 8; ++i) {
        std::string file_name{ ("BasicOnlyFoodSim" + std::to_string(i+4)) };
        file_name += ".csv";
        test_density(base_map_size + i * map_delta, food_radius, file_name);
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
