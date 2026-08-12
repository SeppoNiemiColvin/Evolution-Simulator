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
    assert(test_regions());
    assert(test_resource_finder());
    assert(test_resource_set());

    for (int i{ 0 }; i < 20; ++i) {
        std::cout << test_survivability(1000, 120.0, 1 / 15.0) << '\n';
    }

    for (int run{}; run < 10; ++run) {
        std::string run_piece{ "_run" + std::to_string(run) };

        for (int density{}; density < 8; ++density) {
            const double map_size{ 500.0 + density * 250 };
            std::string file_prefix{ "simulation" + std::to_string(density) + run_piece};
            std::string file{ file_prefix + "_withoutThirst.csv" };
            test_density(map_size, file, false);

            file= file_prefix + "_withThirst.csv";
            test_density(map_size,  file, true);

        }
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
