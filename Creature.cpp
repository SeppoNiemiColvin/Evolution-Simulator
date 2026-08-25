#include "Creature.h"
#include "Environment.h"
#include <iostream>
#include <cmath>
#include <string_view>
#include <utility>
#include "ReproHelpers.h"

Creature::Creature(const Vector2d& position, const Genotype& genes) :
	m_EYESIGHT{ genes.eyesight },
	m_body{position, genes.body_genes},
	m_is_female{genes.is_female}
{
}


void Creature::acc_toward(const Vector2d& target) {
	m_acceleration = target - get_position() - get_velocity();
	//adjust for reach, we will attempt to make it somewhere within range
	m_acceleration *= std::fmax((m_acceleration.norm() - .7 * m_REACH) / m_acceleration.norm(),0);
	m_acceleration -= 1.2 * get_velocity();
}

void Creature::acc_neutral() {
	const double speed{ m_body.get_speed()};

	//we are moving slowly and should choose a direction to move in
	if (speed < .1 * get_max_acceleration()) {
		static std::mt19937_64 m_generator;
		static std::uniform_real_distribution distribution(0.0, 2 * 3.14159);
		const double angle{ distribution(m_generator) };
		const Vector2d direction(cos(angle), sin(angle));
		m_acceleration = get_max_acceleration() * direction;
		return;
	}

	//we can move faster in this direction
	if (speed < get_max_speed()) {
		const double acc_size{ std::fmin(get_max_speed() - speed, get_max_acceleration())};
		m_acceleration = acc_size * get_velocity() / speed;
	}
}



void Creature::move() {
	//figure out desired acceleration
	if (m_goal) {
		for (int i{}; i < 4; ++i) {
			acc_toward(m_goal->get_nearest(get_position()));
			m_body.exert(m_acceleration, 0.25);
		}

	}
	else {
		acc_neutral();
		m_body.exert(m_acceleration);
	}
}

bool Creature::above_threshold(Body::Need need) {
	assert(need != Body::Need::num_needs);
	return m_body.felt_need(need) >= m_THRESHOLDS[need];
}

void Creature::update() {
	//if we are dead, we stay dead
	if (m_state == dead) {
		return;
	}
	m_acceleration = Vector2d{};
	//time passes and the creature's metabolism runs
	m_body.metabolize();
	
	m_age += 1;
	m_time_since_mated += 1;
	//check if state needs to switch and if so adjust the goal
	// state will become dead if any reserve is depleted
	
	if (m_body.is_dead()) {
		m_state = dead;
		return;
	}


	//if the current state matches that states requirement we keep that state
	if (m_state < Body::Need::num_needs && !above_threshold(static_cast<Body::Need>(m_state))) {
		return;
	}

	//else go through and check if the state needs to be updated
	for (int need{}; need < Body::Need::num_needs; ++need) {
		if (!above_threshold(static_cast<Body::Need>(need))) {
			if (m_state != static_cast<State>(need)) {
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



Creature* Creature::reproduce(Creature* other){
	Creature& mate{ *other };

	const double eyesight{ combine_scaled(m_EYESIGHT, mate.m_EYESIGHT) };
	Genotype baby_genes{ m_body.reproduce(mate.m_body), eyesight , coinFlip()};
	return new Creature{ get_position(), baby_genes};
}

//function to make printing positions nicer
std::string string_position(const Vector2d& position) {
	std::string answer{ "( " };
	answer.append(std::to_string(position(0)) + " , ");
	answer.append(std::to_string(position(1)) + " )");
	return answer;
}

void Creature::print() const {
	std::cout << "State:\t\t"	<< m_state << '\n';
	std::cout << "Speed:\t\t"	<< get_max_speed() << '\n';
	std::cout << "Eyesight:\t"	<< m_EYESIGHT << '\n';
	std::cout << "Position:\t"	<< string_position(get_position()) << '\n';
	std::cout << "Veclocity:\t" << string_position(get_velocity()) << '\n';
	std::cout << "Goal:\t\t"	<< (m_goal ? string_position(m_goal->get_position()) : "Null") << '\n';
	std::cout << "Energy:\t\t"	<< m_body.print_reserve(Body::Need::calories) << '\n';
	std::cout << "Water:\t\t"	<< m_body.print_reserve(Body::Need::water) << '\n';
}





bool Creature::goal_in_reach() {
	if (!m_goal) { return false; }

	//note this needs to be checked first so that way the creature releases
	//unavailable goals (especially important so that it doesn't pursue a dead mate)
	if (!(m_goal->is_available())) {
		//if the goal is no longer available drop it as a goal
		m_goal = nullptr;
		return false;
	}
	//creature needs to slow down to mate, eat, drink, ect
	if (m_body.get_speed() > 1.0) {
		return false; 
	}

	return (m_goal->get_distance(get_position())) < m_REACH;
}

void Creature::set_goal(SurvivalResource* new_goal) {
	if (new_goal && new_goal->get_distance(get_position()) < m_EYESIGHT) { m_goal = new_goal; }
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

int Creature::time_till_unavailable() const {
	return m_body.time_till_unavailable(m_THRESHOLDS);
}

int Creature::time_till_dead() const {
	return m_body.time_till_dead();
}

void Creature::Genotype::test_validity() const {
	constexpr std::string_view positivity_error{ "Variable should be strictly positive" };
	using ErrorMessage = std::pair<Gene, std::string_view>;

	body_genes.test_validity();

	if (eyesight <= 0.0) {
		throw ErrorMessage{ Gene::eyesight, positivity_error };
	}
}