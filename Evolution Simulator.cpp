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
#include "ResourceSet.h"

using PopulationIter = std::unordered_set<Creature*>::iterator;

//sets up where we plan to place food with local variables to conserve memory
std::array<FoodSource, 5> initialize_food() {
    std::array<Vector2d,5> foodLocations{
    Vector2d(-50.0, 75.0),
    Vector2d(300.0, 40.0),
    Vector2d(-400.0, -200.0),
    Vector2d(250.0, -100.0),
    Vector2d(100.0, 70.0)
    };

    std::array<FoodSource, 5> answer{ [foodLocations]<auto...I>(std::index_sequence<I...>)
    {
        return std::array<FoodSource,5>{(foodLocations[I])...};
    }(std::make_index_sequence<5>{})
    };
    return answer;
}


void acheive_goal(Creature& gooblet);

int main()
{
    std::mt19937_64 generator;
    std::uniform_real_distribution distribution(-500.0, 500.0);


    ResourceFinder<FoodSource, 5> foodManager{ initialize_food() };
    ResourceSet<Creature> femalePopulation{};

    for (int i{}; i < 3; i++) {
        Creature* new_creature = new Creature{
            Vector2d(distribution(generator), distribution(generator)),
            10.0
        };
        femalePopulation.insert(new_creature);
    }

    Creature gooblet{ Vector2d{}, 10.0 };

    ResourceSet<Creature> malePopulation{};
    malePopulation.insert(new Creature{ Vector2d(200.0,30.0), 10.0 });
    malePopulation.insert(new Creature{ Vector2d(-50.0, -300.0), 10.0 });


    for (PopulationIter iter{ malePopulation.begin() }; iter != malePopulation.end(); ++iter) {
        (*iter)->eat();
    }

    malePopulation.update();

    FoodSource* food{ foodManager.find_closest_to(gooblet.get_position()) };

    std::cout << food->get_position() << std::endl;
     
    gooblet.set_goal(food);

    for (int time{}; time < 75; ++time) {
        std::cout << "Time: " << time << std::endl;
        gooblet.move();

        acheive_goal(gooblet);

        gooblet.update();

        if (gooblet.is_seeking()) {
            switch (gooblet.get_state()) {
            case Creature::State::hungry:
                gooblet.set_goal(foodManager.find_closest_to(gooblet.get_position()));
                break;
            case Creature::State::horny:
                gooblet.set_goal(malePopulation.find_closest_to(gooblet.get_position()));
            }
        }



        gooblet.print();
        std::cout << std::endl;

    }

}

void acheive_goal(Creature& gooblet) {
    if (gooblet.goal_in_reach()) {
        switch (gooblet.get_state()) {
        case Creature::State::hungry:
            gooblet.eat();
            break;
        case Creature::State::horny:
            gooblet.get_goal()->use();
            gooblet.use();
            break;
        }

        gooblet.set_goal(nullptr);
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
