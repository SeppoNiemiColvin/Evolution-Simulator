#include "Creature.h"
#include "Environment.h"
#include <iostream>
#include <cmath>
#include <string_view>

Creature::Creature(Vector2d position, double speed, double eyesight) :
	m_position{ position },
	m_MAX_SPEED{ speed },
	m_EYESIGHT{ eyesight }
{
	m_current_reserves[water] = m_RESERVE_CAPS[water] * m_THRESHOLDS[water];
}

Creature::Creature(Vector2d position, double speed, double acceleration, double eyesight) :
	m_position{ position },
	m_MAX_SPEED{ speed },
	m_MAX_ACCELERATION{acceleration},
	m_EYESIGHT{ eyesight }
{
	m_current_reserves[water] = m_RESERVE_CAPS[water] * m_THRESHOLDS[water];
}

void Creature::acc_toward(const Vector2d& target) {
	m_acceleration = target - m_position - m_velocity;
	//adjust for reach
	m_acceleration *= (m_acceleration.norm() - .9 * m_REACH) / m_acceleration.norm();
	
}

void Creature::acc_neutral() {
	const double speed{ m_velocity.norm() };

	//we are moving slowly and should choose a direction to move in
	if (speed < .1 * m_MAX_ACCELERATION) {
		static std::mt19937_64 m_generator;
		static std::uniform_real_distribution distribution(0.0, 2 * 3.14159);
		const double angle{ distribution(m_generator) };
		const Vector2d direction(cos(angle), sin(angle));
		m_acceleration = m_MAX_ACCELERATION * direction;
		return;
	}

	//we can move faster in this direction
	if (speed < m_MAX_SPEED) {
		const double acc_size{ std::fmin(m_MAX_SPEED - speed, m_MAX_ACCELERATION) };
		m_acceleration = acc_size * m_velocity / speed;
	}
}



void Creature::move() {
	//figure out desired acceleration
	if (m_goal) {
		acc_toward(m_goal->get_nearest(m_position));
	}
	else {
		acc_neutral();
	}
	// move in that direction
	if (m_acceleration.norm() > m_MAX_ACCELERATION) {
		m_acceleration.normalize();
		m_acceleration *= m_MAX_ACCELERATION;
	}
	m_velocity += m_acceleration;
	if (m_velocity.norm() > m_MAX_SPEED) {
		m_velocity.normalize();
		m_velocity *= m_MAX_SPEED;
	}
	m_position += m_velocity;
}

bool Creature::above_threshold(Physical_Need need) {
	assert(need != num_needs);
	return m_current_reserves[need] >= m_THRESHOLDS[need] * m_RESERVE_CAPS[need];
}

void Creature::update() {
	//if we are dead, we stay dead
	if (m_state == dead) {
		return;
	}
	m_acceleration = Vector2d{};
	//time passes and the creature's metabolism runs
	m_current_reserves[calories] -= 1.0;
	m_current_reserves[water] -= 1.0;
	
	//check if state needs to switch and if so adjust the goal
	// state will become dead if any reserve is depleted
	
	for (int i{}; i < num_needs; ++i) {
		if (m_current_reserves[i] <= 0.0) {
			m_state = dead;
			return;
		}
	}

	if (m_state < num_needs && !above_threshold(static_cast<Physical_Need>(m_state))) {
		return;
	}

	for (int need{}; need < num_needs; ++need) {
		if (!above_threshold(static_cast<Physical_Need>(need))) {
			if (m_state != need) {
				m_state = static_cast<State>(need);
				m_goal = nullptr;
			}
			return;
		}
	}

	if (m_state != horny){
		m_state = horny;
		m_goal = nullptr;
	}
}

void Creature::eat() {
	m_current_reserves[calories] = m_RESERVE_CAPS[calories];
}

void Creature::drink() {
	m_current_reserves[water] = m_RESERVE_CAPS[water];
}

Creature* Creature::reproduce(Creature* other) const{
	Creature& mate{ *other };

	static std::mt19937_64 generator;
	static std::normal_distribution distribution(0.0, 0.5);

	const double baby_speed{ (m_MAX_SPEED + mate.m_MAX_SPEED) / 2 + distribution(generator) };
	double acceleration{ (m_MAX_ACCELERATION + mate.m_MAX_ACCELERATION) / 2 + .5 * distribution(generator) };
	acceleration = std::fmin(baby_speed / 2, acceleration);
	const double eyesight = (m_EYESIGHT + mate.m_EYESIGHT) / 2 + 5*distribution(generator);

	return new Creature{ m_position, baby_speed, acceleration, eyesight };
}

//function to make printing positions nicer
std::string string_position(const Vector2d& position) {
	std::string answer{ "( " };
	answer.append(std::to_string(position(0)) + " , ");
	answer.append(std::to_string(position(1)) + " )");
	return answer;
}

void Creature::print() const {
	std::cout << "State:\t\t" << m_state << '\n';
	std::cout << "Speed:\t\t" << m_MAX_SPEED << '\n';
	std::cout << "Eyesight:\t" << m_EYESIGHT << '\n';
	std::cout << "Position:\t" << string_position(m_position) << '\n';
	std::cout << "Veclocity:\t" << string_position(m_velocity) << '\n';
	std::cout << "Goal:\t\t" << (m_goal ? string_position(m_goal->get_position()) : "Null") << '\n';
	std::cout << "Energy:\t\t" << m_current_reserves[calories] << '/' << m_RESERVE_CAPS[calories] << '\n';
	std::cout << "Water:\t\t" << m_current_reserves[water] << '/' << m_RESERVE_CAPS[water] << '\n';
}





bool Creature::goal_in_reach() {
	if (!m_goal) { return false; }

	if (!(m_goal->is_available())) {
		//if the goal is no longer available drop it as a goal
		m_goal = nullptr;
		return false;
	}

	return (m_goal->get_distance(m_position)) < m_REACH;
}

void Creature::set_goal(SurvivalResource* new_goal) {
	if (new_goal && new_goal->get_distance(m_position) < m_EYESIGHT) { m_goal = new_goal; }
}

constexpr std::string_view getStateName(Creature::State state) {
	switch (state) {
	case Creature::State::thirsty:	return "Thirsty";
	case Creature::State::hungry:	return "Hungry";
	case Creature::State::horny:	return "Horny";
	case Creature::State::dead:		return "Dead";
	default:						return "Other";
	}
}

std::ostream& operator<<(std::ostream& out, Creature::State state) {
	return out << getStateName(state);
}