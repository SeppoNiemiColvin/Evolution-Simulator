#include "FoodSource.h"
#include <iostream>
//to be filled in with update logic, which will likely be a little more complicated
void FoodSource::update() {
	;
}

void FoodSource::print() const {
	std::cout << "Fruit " << m_fruits << " at ";
	std::cout << m_position(0) << " , " << m_position(1) << std::endl;
}