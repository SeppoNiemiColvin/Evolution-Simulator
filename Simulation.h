#pragma once
#include "ResourceFinder.h"
class Creature;
class FoodSource;

template<auto foodLength>
class Simulation
{
private:
	ResourceFinder<FoodSource, foodLength> foodManager;
};

