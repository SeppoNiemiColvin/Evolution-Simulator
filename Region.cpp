#include "Region.h"
#include "Creature.h"

void Region::pull_inside(Creature* creature) const {
	Vector2d position{ creature->get_position() };
	Vector2d displacement{ get_nearest(position) - position };
	creature->m_position += (1.05 * displacement);
	constexpr double EPSILON = 1e-8;
	if (std::abs(displacement(0)) > EPSILON) {
		creature->m_velocity(0) *= -1;
	}
	
	if (std::abs(displacement(1)) > EPSILON) {
		creature->m_velocity(1) *= -1;
	}
}