#include "Region.h"
#include "Creature.h"

void Region::pull_inside(Creature* creature) const {
	Vector2d position{ creature->get_position() };
	Vector2d displacement{ get_nearest(position) - position };
	creature->m_position += (1.05 * displacement);
}