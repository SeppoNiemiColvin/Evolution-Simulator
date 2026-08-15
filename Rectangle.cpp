#include "Rectangle.h"
#include "Creature.h"

#include<random>
#include<array>
#include<cmath>

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
	double x_distance_out{ position(0) <= mid_x ? position(0) - m_left : m_right - position(0) };
	if (x_distance_out < 0.0) {
		return;
	}
	double y_distance_out{ position(1) <= mid_y ? position(1) - m_lower : m_upper - position(1) };
	if (y_distance_out < 0.0) {
		return;
	}

	//based on which side is closer bounce the creature out from that direction
	if (x_distance_out < y_distance_out && position(0) <= mid_x) {
		position(0) = m_left;
		creature->m_body.m_velocity(0) *= -1;
	}
	else if (x_distance_out < y_distance_out) {
		position(0) = m_right;
		creature->m_body.m_velocity(0) *= -1;
	}
	else if (position(1) <= mid_y) {
		position(1) = m_lower;
		creature->m_body.m_velocity(1) *= -1;
	}
	else {
		position(1) = m_upper;
		creature->m_body.m_velocity(1) *= -1;
	}
	creature->m_body.m_position = position;
}

//stretches a point in the interval (0,1) to be in the interval (lower, upper)
//this will allow us to have a single static pseudo random number generator +
//distribution to use across various rectangles
double stretch(double point,double lower, double upper) {
	return (upper - lower) * point + lower;
}

Vector2d Rectangle::random_point() const {
	static std::mt19937_64 generator;
	static std::uniform_real_distribution random(0.0, 1.0);
	return Vector2d(stretch(random(generator), m_left, m_right),
		stretch(random(generator), m_lower, m_upper));
}

bool Rectangle::intersects(const Rectangle& rectangle) const {
	const std::array<Vector2d, 4> corners{
		Vector2d(rectangle.get_left(), rectangle.get_lower()),
		Vector2d(rectangle.get_left(), rectangle.get_upper()),
		Vector2d(rectangle.get_right(), rectangle.get_lower()),
		Vector2d(rectangle.get_right(), rectangle.get_upper())
	};
	for (const auto& corner : corners) {
		if (is_inside(corner)) {
			return true;
		}
	}
	return rectangle.is_inside(Vector2d(m_left, m_right));
}

Rectangle operator+(const Rectangle& rect, const Vector2d& displacement) {
	return Rectangle{
		rect.m_left + displacement(0),
		rect.m_right + displacement(0),
		rect.m_lower + displacement(1),
		rect.m_upper + displacement(1) };
}

/*
* consider the line segment from start in direction delta
* attempts to find a point on the segment so that point(axis) = value
* and returns the value of the other axis if it exists.
*/
double* line_segment_with_value(const Vector2d& start, const Vector2d& delta, const double value, int axis) {
	const int return_axis{ 1 - axis };
	//handle vertical case
	if (delta(axis) == 0 ){
		return nullptr;
	}

	const double time{(value - start(axis)) / delta(axis) };
	if (time < 0.0 || time > 1.0) {
		return nullptr;
	}

	return new double(start(return_axis) + delta(return_axis) * time);
}
bool Rectangle::line_intersects(const Vector2d& start, const Vector2d& end) const {
	if (is_inside(start) || is_inside(end)) {
		return true;
	}
	//at this point we can assume both start and end are outside
	//so we only have to check if the line segment hits the rectangle's sides
	const Vector2d delta{ end - start };
	if (delta(0) == 0) {
		const double upper{ std::fmax(end(1), start(1)) };
		const double lower{ std::fmin(end(1), start(1)) };
		return upper >= m_upper && lower <= m_lower;
	}
	else {
		double* ycheck{ line_segment_with_value(start, delta, m_left,0) };
		if (ycheck && *ycheck >= m_lower && *ycheck <= m_upper) {
			return true;
		}
		delete ycheck;
		ycheck = line_segment_with_value(start, delta, m_right, 0);
		if (ycheck && *ycheck >= m_lower && *ycheck <= m_upper) {
			return true;
		}
		delete ycheck;
	}
	if (delta(1) == 0) {
		const double left{ std::fmin(start(0), end(0)) };
		const double right{ std::fmax(start(0), end(0)) };
		return right >= m_right && m_left >= left;
	}
	else {
		double* xcheck{ line_segment_with_value(start, delta, m_lower, 1) };
		if (xcheck && *xcheck >= m_left && *xcheck <= m_right) {
			return true;
		}
		xcheck = line_segment_with_value(start, delta, m_upper, 1);
		if (xcheck && *xcheck >= m_left && *xcheck <= m_right) {
			return true;
		}
	}
	return false;
}