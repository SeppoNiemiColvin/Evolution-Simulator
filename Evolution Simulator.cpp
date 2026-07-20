// Evolution Simulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Eigen/Dense>
#include <unordered_set>
#include <random>
#include <vector>
#include <array>

#include "Simulation.h"
#include "HalfPlane.h"

void acheive_goal(Creature& gooblet);

int main()
{
    double map_size{ 1e3 };
    double food_radius{ 30.0 };
    double food_area{ food_radius * food_radius * 3.14159 };
    int food_number{ static_cast<int>(map_size * map_size / food_area) };
    std::cout << food_number << std::endl;
    Simulation simulation{ -map_size, map_size, -map_size, map_size, 10.0, 4 * food_radius, 20,food_number };

    Creature* gooblet = new Creature(Vector2d(10.0, 10.0), 10.0, 150.0);
    gooblet->print();

    const HalfPlane left{ Vector2d(1,0), Vector2d{} };
    const HalfPlane bottom{ Vector2d(0,1) , Vector2d{} };
    const HalfPlane right{ Vector2d(-1,0), Vector2d(30.0, 0.0) };
    const HalfPlane top{ Vector2d(0,-1), Vector2d(0, 30.0) };
    const HalfPlane diagonal(Vector2d(-1, -1), Vector2d(20.0, 20.0));
    const std::array<HalfPlane, 5> walls{ left, right, bottom, top, diagonal};
    std::cout << diagonal.point_on_line();
    for (int timer{}; timer < 20; ++timer) {
        gooblet->move();
        std::cout << "Timer: " << timer << std::endl;
        std::cout << gooblet->get_position() << std::endl;
        for (auto& wall : walls) {
            wall.push_out(gooblet);
        }
        std::cout << gooblet->get_position() << std::endl << std::endl;
    }


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
