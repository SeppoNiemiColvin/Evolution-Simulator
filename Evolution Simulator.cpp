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
    double map_size{ 5e3 };
    double food_radius{ 15.0 * 5.0};
    double food_area{ food_radius * food_radius * 3.14159 };
    int food_number{ static_cast<int>(map_size * map_size / food_area) };
    std::cout << food_number << std::endl;

    Simulation simulation{ -map_size, map_size, -map_size, map_size,
        10.0, 100.0, 20, food_number };

    //request a file name
    std::cout << "Enter a file name: ";
    std::string file_name{};
    std::cin >> file_name;

    //set up an output file to receive the simulation data
    std::ofstream output_file{ file_name };
    if (!output_file) {
        std::cerr << "Unable to open output file\n";
        return 1;
    }

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


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
