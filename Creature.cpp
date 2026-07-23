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
}

Creature::Creature(Vector2d position, double speed, double acceleration, double eyesight) :
	m_position{ position },
	m_MAX_SPEED{ speed },
	m_ACCELERATION{acceleration},
	m_EYESIGHT{ eyesight }
{
}

void Creature::move_toward(const Vector2d& target) {
	const Vector2d DELTA{ target - m_position - m_velocity };
	//get distance so we can normalize
	const double DISTANCE{ DELTA.norm()};
	if (DISTANCE <= m_ACCELERATION) {
		m_velocity += DELTA;
	}
	else {
		m_velocity += m_ACCELERATION * DELTA / DISTANCE;
	}
	//make sure we aren't exceeding our max velocity
	if (m_velocity.norm() > m_MAX_SPEED) {
		m_velocity *= m_MAX_SPEED / m_velocity.norm();
	}
}

void Creature::move() {
	if (m_goal) { move_toward(m_goal->get_position()); }
	else { move_neutral(); }
	m_position += m_velocity;
}

void Creature::update() {
	//if we are dead we stay dead
	if (m_state == dead) {
		return;
	}
	//time passes and the creature's metabolism runs
	m_hunger -= 1.0;
	
	//check if state needs to switch and if so adjust the goal
	if (m_hunger <= 0.0) {
		m_state = dead;
		return;
	}
	if (m_hunger <= m_HUNGER_THRESHOLD && m_state != hungry) {
		m_state = hungry;
		m_goal = nullptr;
		return;
	}

	if (m_hunger > m_HUNGER_THRESHOLD && m_state != horny){
		m_state = horny;
		m_goal = nullptr;
	}
}

void Creature::eat() {
	m_hunger = m_MAX_HUNGER;
}

Creature* Creature::reproduce(Creature* other) const{
	Creature& mate{ *other };

	static std::mt19937_64 generator;
	static std::normal_distribution distribution(0.0, 0.5);

	const double baby_speed{ (m_MAX_SPEED + mate.m_MAX_SPEED) / 2 + distribution(generator) };
	double acceleration{ (m_ACCELERATION + mate.m_ACCELERATION) / 2 + .5 * distribution(generator) };
	acceleration = std::fmin(baby_speed / 2, acceleration);
	const double eyesight = (m_EYESIGHT + mate.m_EYESIGHT) / 2 + 5*distribution(generator);

	return new Creature{ m_position, baby_speed, acceleration, eyesight };
}

std::string string_position(const Vector2d& position) {
	std::string answer{ "( " };
	answer.append(std::to_string(position(0)) + " , ");
	answer.append(std::to_string(position(1)) + " )");
	return answer;
}
void Creature::print() const {
	std::cout << "State " << m_state << '\n';
	std::cout << "Speed: " << m_MAX_SPEED << '\n';
	std::cout << "Eyesight: " << m_EYESIGHT << '\n';
	std::cout << "Position: " << string_position(m_position) << '\n';
	std::cout << "Goal: " << (m_goal ? string_position(m_goal->get_position()) : "Null") << '\n';
	std::cout << "Hunger: " << m_hunger << '/' << m_MAX_HUNGER << '\n';
}



void Creature::move_neutral() {
	const double speed{ m_velocity.norm() };

	//we are moving slowly and should choose a direction to move in
	if (speed < .1 * m_ACCELERATION) {
		static std::mt19937_64 m_generator;
		static std::uniform_real_distribution distribution(0.0, 2 * 3.14159);
		const double angle{ distribution(m_generator) };
		m_velocity(0) += m_ACCELERATION * cos(angle);
		m_velocity(1) += m_ACCELERATION * sin(angle);
		return;
	}
	
	//we can move faster in this direction
	if (speed < m_MAX_SPEED) {
		const double acc_size{ std::fmin(m_MAX_SPEED - speed, m_ACCELERATION) };
		m_velocity += acc_size * m_velocity / speed;
	}
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
	case Creature::State::hungry:	return "Hungry";
	case Creature::State::horny:	return "Horny";
	case Creature::State::dead:		return "Dead";
	default:						return "Other";
	}
}

std::ostream& operator<<(std::ostream& out, Creature::State state) {
	return out << getStateName(state);
}