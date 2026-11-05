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
	m_brain{genes.brain_genes}
{
}



void Creature::acc_toward(const Vector2d& target) {
	m_acceleration = target - get_position() - get_velocity();
	//adjust for reach, we will attempt to make it somewhere within range
	m_acceleration *= std::fmax((m_acceleration.norm() - .7 * s_REACH) / m_acceleration.norm(),0);
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


void Creature::manage_view(ForceManager::View& surroundings) const {
	for (auto& type : surroundings) {
		//update the displacement vectors
		for (auto& observations : type) {
			observations.from_perspective(get_position());
		}

		//make sure they are sorted by the new sitance
		auto sort_key{
			[](const MovementAnalyzer::Observation& a, const MovementAnalyzer::Observation& b) {
				return a.distance < b.distance;}
		};
		std::sort(type.begin(), type.end(), sort_key);
	}

}

ForceManager::View Creature::make_view(const Surroundings& surroundings) {
	
	ForceManager::View view{ };

	m_goal.active = false;

	for (auto flag : m_brain.get_flags()) {
		const bool empty{ !surroundings.contains(flag) || surroundings.at(flag).empty() };
		view.push_back(empty ? std::vector<Observation>{} : surroundings.at(flag));	
		if (empty) {
			continue;
		}
		// if there are things in here then we sort them
		auto sort_key{
			[](const Observation& a, const Observation& b) {
				return a.distance < b.distance;}
		};
		std::vector<Observation>& new_line{ view.back() };
		std::sort(new_line.begin(), new_line.end(), sort_key);
		update_goal(flag, new_line[0]);
	}
	return view;
}

bool Creature::should_replace_goal(Body::Need need, double distance) const {
	bool desire{m_body.felt_need(need) < s_SEEKING_THRESHOLDS[need] };
	//new resource has to be something the creature is willling to use and closer than the current goal
	return desire && (!m_goal.active || (m_goal.get_distance() > distance));
}


void Creature::update_goal(Ecology flag, const Observation& potential_goal) {
	MetaEcology relationship{ m_brain.get_relationship(flag) };
	//flag determines which need will be checked against
	switch (relationship) {
	case MetaEcology::water:
		if (should_replace_goal(Body::Need::water, potential_goal.distance)) {
			set_goal(potential_goal.object, relationship);
		}
		break;

	case MetaEcology::food:
		if (should_replace_goal(Body::Need::calories, potential_goal.distance)) {
			set_goal(potential_goal.object, relationship);
		}
		break;
	case MetaEcology::mate:
		if (is_available() && (!m_goal.active || potential_goal.distance < m_goal.get_distance()) ) {
			set_goal(potential_goal.object, relationship);
		}
		break;
	}
}


void Creature::move(const Surroundings& surroundings) {
	ForceManager::View view{ make_view(surroundings)};
	//figure out desired acceleration
	if (!surroundings.empty()) {
		constexpr int steps{ 10 };
		constexpr double fraction{ 1.0 / steps };
		//take some individual steps using the view given.
		for (int i{}; i < steps; ++i) {
			Body::Status status{ m_body.make_status()};
			m_acceleration = m_brain.response(status, view);
			m_body.exert(m_acceleration, fraction);
			manage_view(view);
			
		}

		//at this point we could use this to determine what actions might be available
		//at this point we are doing that by finding the closest 
		ForceManager::Flags flags{ m_brain.get_flags() };
		for (int i{}; i < flags.size(); ++i) {
			if (view[i].empty()) {
				continue;
			}
			update_goal(flags[i], view[i][0]);
		}
	}
	//note make_view will leave m_goal as a nullpointer if none of the objects
	//within eyesight are above the seeking threshold, so this only runs if it doesn't
	//see anything interesting
	if (!m_goal.active) {
		acc_neutral();
		m_body.exert(m_acceleration);
	}
}

bool Creature::above_threshold(Body::Need need) {
	assert(need != Body::Need::num_needs);
	return m_body.felt_need(need) >= s_MATING_THRESHOLDS[need];
}

void Creature::update() {
	//if we are dead, we stay dead
	if (m_body.is_dead()) {
		return;
	}
	m_acceleration = Vector2d{};
	//time passes and the creature's metabolism runs
	m_body.metabolize();
	
	m_age += 1;
	m_time_since_mated += 1;
	//check if state needs to switch and if so adjust the goal
	// state will become dead if any reserve is depleted

}



Creature* Creature::reproduce(Creature* other){
	Creature& mate{ *other };

	const double eyesight{ combine_scaled(m_EYESIGHT, mate.m_EYESIGHT) };
	Genotype baby_genes{ m_brain.reproduce(mate.m_brain), m_body.reproduce(mate.m_body), eyesight };
	return new Creature{ get_position(), baby_genes };
}

//function to make printing positions nicer
std::string string_position(const Vector2d& position) {
	std::string answer{ "( " };
	answer.append(std::to_string(position(0)) + " , ");
	answer.append(std::to_string(position(1)) + " )");
	return answer;
}

void Creature::print() const {
	std::cout << "Speed:\t\t"	<< get_max_speed() << '\n';
	std::cout << "Eyesight:\t"	<< m_EYESIGHT << '\n';
	std::cout << "Position:\t"	<< string_position(get_position()) << '\n';
	std::cout << "Veclocity:\t" << string_position(get_velocity()) << '\n';
	std::cout << "Goal:\t\t"	<< (m_goal.active ? string_position(m_goal.get_position()) : "Null") << '\n';
	std::cout << "Energy:\t\t"	<< m_body.print_reserve(Body::Need::calories) << '\n';
	std::cout << "Water:\t\t"	<< m_body.print_reserve(Body::Need::water) << '\n';
}





bool Creature::goal_in_reach() {
	if (!m_goal.active) { return false; }

	//note this needs to be checked first so that way the creature releases
	//unavailable goals (especially important so that it doesn't pursue a dead mate)
	if (!(m_goal.is_available())) {
		//if the goal is no longer available drop it as a goal
		m_goal.active = false;
		return false;
	}
	//creature needs to slow down to mate, eat, drink, ect
	if (m_body.get_speed() > 5.0) {
		return false; 
	}

	return (m_goal.get_distance()) < s_REACH;
}


int Creature::time_till_unavailable() const {
	return m_body.time_till_unavailable(s_MATING_THRESHOLDS);
}

int Creature::time_till_dead() const {
	return m_body.time_till_dead();
}

std::string Creature::Genotype::header() const {
	return Body::Genotype::header() + brain_genes.header() + "eyesight";
}


std::string Creature::Genotype::as_string() const {
	return body_genes.to_string() + brain_genes.as_string() + std::to_string(eyesight);
}

void Creature::Genotype::print_to_file(std::ofstream& file) const {
	file << as_string();
}
void Creature::Genotype::test_validity() const {
	constexpr std::string_view positivity_error{ "Variable should be strictly positive" };
	using ErrorMessage = std::pair<Gene, std::string_view>;

	body_genes.test_validity();

	if (eyesight <= 0.0) {
		throw ErrorMessage{ Gene::eyesight, positivity_error };
	}
}

Creature::Genotype operator+(const Creature::Genotype& genes1, const Creature::Genotype& genes2) {
	return Creature::Genotype{
		genes1.brain_genes + genes2.brain_genes,
		genes1.body_genes + genes2.body_genes,
		genes1.eyesight + genes2.eyesight
		
	};
}
Creature::Genotype operator/(const Creature::Genotype& genes, const double denominator) {
	return Creature::Genotype{
		genes.brain_genes / denominator,
		genes.body_genes / denominator,
		genes.eyesight / denominator
		
	};
}

Creature::Genotype Creature::get_genotype() const {
	return Genotype{ m_brain.get_genotype(), m_body.get_genes(), m_EYESIGHT };
}

Creature::Genotype Creature::Genotype::zero(Species structure) {
	return Genotype{ ForceManager::Genotype::zero(structure), Body::Genotype::zero(), 0.0 };
}

void Creature::Genotype::operator+=(const Creature::Genotype& genes) {
	brain_genes += genes.brain_genes;
	body_genes += genes.body_genes;
	eyesight += genes.eyesight;
}

void Creature::Genotype::clear() {
	brain_genes.clear();
	body_genes.clear();
	eyesight = 0.0;
}