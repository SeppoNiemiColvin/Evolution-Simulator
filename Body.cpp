 #include "Body.h"
#include "ReproHelpers.h"
#include <cmath>
#include <vector>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>


Body::Body(Vector2d position, Genotype genes) :
	m_RESERVE_CAPS{genes.reserve_caps},
	m_position{ position },
	m_MAX_SPEED{ genes.max_speed },
	m_MAX_ACCELERATION{ genes.max_acceleration},
	m_METABOLIC_BASE{ create_metabolic_base(genes) },
	m_IGNORED_NEEDS{genes.ignored_needs},
	m_current_reserves{genes.starting_amounts}
{
	//makes sure the genes we entered were sensical
	assert(genes.max_speed > 0 && "Speed needs to be positive");
	assert(genes.max_acceleration > 0 && "Acceleration needs to be positive");
	for (int need{}; need < num_needs; ++need) {
		assert(genes.ignored_needs.contains(static_cast<Need>(need))
			|| genes.reserve_caps[need] > 0 && "Used needs need to be positive to survive");
		assert(genes.starting_amounts[need] > 0 && "Starting amount needs to be positive to survive");
	}
}

void Body::exert( Vector2d& acceleration, double delta) {
	// move in that direction
	if (acceleration.norm() > m_MAX_ACCELERATION) {
		acceleration.normalize();
		acceleration *= m_MAX_ACCELERATION;
	}
	m_velocity += delta * acceleration;
	if (m_velocity.norm() > m_MAX_SPEED) {
		m_velocity.normalize();
		m_velocity *= m_MAX_SPEED;
	}
	m_position += delta * m_velocity;
}

void Body::metabolize() {
	//most metabolic information will be contained in the metabolic base info
	//but might eventually include other information such as current velocity/temperature, ect
	for (int need{}; need < num_needs; ++need) {
		m_current_reserves[need] -= m_METABOLIC_BASE[need];
	}
}


std::string Body::print_reserve(Need need) const {
	return std::to_string(m_current_reserves[need]) + "/" + std::to_string(m_RESERVE_CAPS[need]);
}

bool Body::is_dead() const {
	for (int i{}; i < num_needs; ++i) {
		if (m_current_reserves[i] <= 0.0) {
			return true;
		}
	}
	return false;
}

int Body::time_till_unavailable(std::array<double, num_needs>& thresholds) const {
	//we are using a pointer here to delay initializing until we find a need that has 
	double* remaining{};
	for (int i{ }; i < num_needs; ++i) {
		const double EPSILON{ 1e-8};
		if (abs(m_METABOLIC_BASE[i]) < EPSILON) {
			continue;
		}
		const double reserve_remaining{ m_current_reserves[i] - thresholds[i] * m_RESERVE_CAPS[i] };
		if (remaining) {
			*remaining = fmin(*remaining, reserve_remaining / m_METABOLIC_BASE[i]);
		}
		else {
			remaining = new double{ reserve_remaining / m_METABOLIC_BASE[i] };
		}
		
	}
	if (!remaining) {
		return -1;
	}
	else {
		const int answer{ static_cast<int>(floor(*remaining)) +1};
		delete remaining;
		return answer;
	}
}

int Body::time_till_dead() const {
	double* remaining{};
	for (int i{}; i < num_needs; ++i) {
		const double EPSILON{ 1e-8 };
		if (abs(m_METABOLIC_BASE[i]) < EPSILON) {
			continue;
		}
		const double reserve_remaining{ m_current_reserves[i] / m_METABOLIC_BASE[i] };
		if (remaining) {
			*remaining = fmin(*remaining, reserve_remaining);
		}
		else {
			remaining = new  double{ reserve_remaining };
		}
	}
	if (remaining) {
		const int answer{ static_cast<int>(ceil(*remaining)) };
		delete remaining;
		return answer;
	}
	else {
		return -1;
	}
}

 
Body::Genotype Body::get_genes() const {
	return Genotype{
		m_RESERVE_CAPS,
		m_MAX_SPEED,
		m_MAX_ACCELERATION,
		m_IGNORED_NEEDS,
	};
}

Body::Genotype Body::reproduce(Body& mate_body){
	Genotype baby_genes{ get_genes() };

	//here we can flip on and off which things we are evolving by which methods
	baby_genes.max_speed = combine_scaled(m_MAX_SPEED, mate_body.m_MAX_SPEED);
	baby_genes.max_acceleration = combine_scaled(m_MAX_ACCELERATION, mate_body.m_MAX_ACCELERATION);

	const double yolk{ baby_genes.starting_amounts[calories] };

	//deplete reserves based on how much we are giving to the babies
	m_current_reserves[calories]			-= .52 * yolk;
	mate_body.m_current_reserves[calories]	-= .52 * yolk;

	return baby_genes;
}

Body::NeedList create_metabolic_base(Body::Genotype& genes) {
	Body::NeedList metabolic_base{};
	for (int need{}; need < Body::Need::num_needs; ++need) {
		if (!genes.ignored_needs.contains(static_cast<Body::Need>(need))) {
			metabolic_base[need] = 1.0;
		}
	}
	return metabolic_base;
}



Body::Status Body::make_status() const {
	NeedVector needs;
	for (int need{}; need < num_needs; ++need) {
		needs[need] = felt_need(static_cast<Need>(need));
	}

	return Status{ needs, m_velocity };
}
void Body::Genotype::test_validity() const {
	constexpr std::string_view positive_error{ "Variable needs to be positive" };

	using ErrorMessage = std::pair<Gene, std::string_view>;

	//check the need lists for problems
	for (int need{}; need < num_needs; ++need) {
		if (reserve_caps[need] <= 0.0) {
			throw ErrorMessage{ Gene::reserve_cap, positive_error };
		}

		if (starting_amounts[need] <= 0.0) {
			throw ErrorMessage{ Gene::starting_amount, positive_error };
		}
	}

	if (max_speed <= 0.0) {
		throw ErrorMessage{ Gene::max_speed, positive_error };
	}

	if (max_acceleration <= 0.0) {
		throw ErrorMessage{ Gene::max_acceleration, positive_error };
	}
}