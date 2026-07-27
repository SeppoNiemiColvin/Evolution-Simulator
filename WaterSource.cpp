#include "WaterSource.h"

void WaterSource::print() const{
	std::cout << "Pond " << m_pond.get_left() << " <= x <= " << m_pond.get_right();
	std::cout << '\t';
	std::cout << " " << m_pond.get_lower() << " <= y <= " << m_pond.get_upper();
	std::cout << '\n';
}

bool overlap(std::vector<WaterSource>& current_water, Rectangle new_pond) {
	for (const auto& pond : current_water) {
		if (pond.m_pond.intersects(new_pond)) {
			return true;
		}
	}
	return false;
}