#include "Creature.h"
#include "Environment.h"
#include <iostream>
#include <cmath>

Creature::Creature(Vector2d position, double speed, double eyesight) :
	m_position{ position },
	m_SPEED{ speed },
	m_EYESIGHT{ eyesight }
{
}

void Creature::move_toward(const Vector2d& target) {
	const Vector2d DELTA{ target - m_position };
	//get distance so we can normalize
	const double DISTANCE{ DELTA.norm()};
	if (DISTANCE <= m_SPEED) {
		m_position = target;
		return;
	}
	m_position += m_SPEED * DELTA / DISTANCE;
}

void Creature::move() {
	if (m_goal) { move_toward(m_goal->get_position()); }
	else { move_neutral(); }
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

	double baby_speed = (m_SPEED + mate.m_SPEED) / 2 + distribution(generator);
	double eyesight = (m_EYESIGHT + mate.m_EYESIGHT) / 2 + distribution(generator);

	return new Creature{ m_position, baby_speed, eyesight };
}

std::string string_position(const Vector2d& position) {
	std::string answer{ "( " };
	answer.append(std::to_string(position(0)) + " , ");
	answer.append(std::to_string(position(1)) + " )");
	return answer;
}
void Creature::print() const {
	std::cout << "Species: " << m_SPECIES << std::endl;
	std::cout << "State " << m_state << std::endl;
	std::cout << "Speed: " << m_SPEED << std::endl;
	std::cout << "Position: " << string_position(m_position) << std::endl;
	std::cout << "Goal: " << (m_goal ? string_position(m_goal->get_position()) : "Null") << std::endl;
	std::cout << "Hunger: " << m_hunger << '/' << m_MAX_HUNGER << std::endl;
}


/*void Creature::move() {
	switch (m_state) {
	case hungry:
		Environment::FoodSource* food{ m_map->nearest_food(m_position) };
		if (!food) {
			move_neutral();
			return;
		}
		const Vector2d food_position{ food->get_position() };
		const double distance{ (food_position - m_position).norm() };
		if (distance < m_REACH) {
			food->used();
		}
		else {
			move_toward(food_position);
		}
		return;
	default:
		move_neutral();
	}

}*/

void Creature::move_neutral() {
	//choose a random direction to walk
	static std::mt19937_64 generator;
	static std::uniform_real_distribution distribution(0.0, 3.14159);
	const double angle{ distribution(generator) };
	m_position(0) += m_SPEED * cos(angle);
	m_position(1) += m_SPEED * sin(angle);
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