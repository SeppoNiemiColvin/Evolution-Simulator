#include "HalfPlane.h"
#include "Creature.h"

#include <cmath>
HalfPlane::HalfPlane(Vector2d normal, double affine_adjustment) :
	m_normal{normal/normal.norm()},
	m_affine{affine_adjustment}
{
	;
}

HalfPlane::HalfPlane(Vector2d normal, Vector2d point):
	m_normal{normal/normal.norm()},
	m_affine{ normal.dot(point)/normal.norm()} {
	;
}

HalfPlane::HalfPlane() { ; }

bool HalfPlane::is_inside(const Vector2d& position) const {
	return m_normal.dot(position) <= m_affine;
}

Vector2d HalfPlane::point_on_line() const {
	if (m_normal(0) == 0) {
		return Vector2d(0, m_affine/m_normal(1));
	}
	return Vector2d(m_affine / m_normal(0), 0);
}

double HalfPlane::get_distance(const Vector2d& position) const {
	const Vector2d difference{ position - point_on_line() };
	return std::fmax(difference.dot(m_normal), 0.0);

}

Vector2d HalfPlane::get_nearest(const Vector2d& position) const {
	return position - get_distance(position) * m_normal;
}

void HalfPlane:: push_out(Creature* creature) const {
	const Vector2d difference{ point_on_line() - creature->get_position() };
	const double distance_out{ std::fmax(difference.dot(m_normal), 0) };
	creature->m_position += ( distance_out * 1.1  * m_normal);
}
