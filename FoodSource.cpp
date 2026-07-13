#include "FoodSource.h"
#include <iostream>
//to be filled in with update logic, which will likely be a little more complicated
void FoodSource::update() {
	++m_timer;
	m_timer %= m_GROWING_TIME;

	while (!m_growing.empty() && m_timer == m_growing.front()) {
		m_growing.pop();
		++m_fruits;
	}
}

void FoodSource::use() {
	--m_fruits;
	m_growing.push(m_timer);
}

void FoodSource::print() const {
	std::cout << "Fruit " << m_fruits << " at ";
	std::cout << m_position(0) << " , " << m_position(1) << std::endl;
}