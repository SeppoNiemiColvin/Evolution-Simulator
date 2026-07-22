#include "Rectangle.h"
#include "Creature.h"

#include<random>
Rectangle::Rectangle() { ; }

Rectangle::Rectangle(double left, double right, double lower, double upper) :
	m_left{ left },
	m_right{ right },
	m_lower{ lower },
	m_upper{ upper }
{
	;
}

bool Rectangle::is_inside(const Vector2d& position) const {
	return (
		m_left <= position(0) &&
		position(0) <= m_right &&
		m_lower <= position(1) &&
		position(1) <= m_upper
		);
}
Vector2d Rectangle::get_nearest(const Vector2d& position) const {
	Vector2d answer{ position };
	if (position(0) < m_left) {
		answer(0) = m_left;
	}
	if (position(0) > m_right) {
		answer(0) = m_right;
	}
	if (position(1) < m_lower) {
		answer(1) = m_lower;
	}
	if (position(1) > m_upper) {
		answer(1) = m_upper;
	}
	return answer;
}

void Rectangle::push_out(Creature* creature) const {
	//This will need to be made more complicated to deal with velocity
	Vector2d position{ creature->get_position() };
	const double mid_x{ (m_left + m_right) / 2 };
	const double mid_y{ (m_upper + m_lower) / 2 };
	if (m_left < position(0) && position(0) <= mid_x) {
		position(0) = m_left;
		creature->m_velocity(0) *= -1;
	}
	if (mid_x < position(0) && position(0) < m_right) {
		position(0) = m_right;
		creature->m_velocity(0) *= -1;
	}
	if (m_lower < position(1) && position(1) <= mid_y) {
		position(1) = m_lower;
		creature->m_velocity(1) *= -1;
	}
	if (mid_y < position(1) && position(1) < m_upper) {
		position(1) = m_upper;
		creature->m_velocity(1) *= -1;
	}
	creature->m_position = position;
}

Vector2d Rectangle::random_point() const {
	static std::mt19937_64 generator;
	static std::uniform_real_distribution vertical(m_lower, m_upper);
	static std::uniform_real_distribution horizontal(m_left, m_right);
	return Vector2d(horizontal(generator), vertical(generator));
}