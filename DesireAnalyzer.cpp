#include "DesireAnalyzer.h"
#include "SurvivalResource.h"

DesireAnalyzer::DesireAnalyzer(const LinearGenotype* genes) :
	m_COEFFICIENTS{ genes->coefficients},
	m_OFFSET{ genes->offset },
	m_EXPONENT{ genes->exponent}
{}


//the genotype stores the force coefficients, then the force offset, then friction coef, then friction_offset
DesireAnalyzer::MetaGenotype* DesireAnalyzer::get_genotype() const {
	return  new LinearGenotype{ AnalyzerType::desire ,
		 m_COEFFICIENTS, m_OFFSET, m_EXPONENT
	};
}

Vector2d DesireAnalyzer::calculate_acceleration(const Observation& observation) const {
	constexpr double lift{ 0.5 };
	if (observation.distance < 5.0) {
		constexpr double scale_match{ (1.0 + lift) / 5.0 };
		return observation.displacement * m_consolidated_force * scale_match;
	}
	else {
		const double force{ m_consolidated_force * ( pow(observation.distance / 5.0, m_EXPONENT) + lift) };
		return observation.displacement * force / observation.distance;
	}

}

void DesireAnalyzer::consolidate(const Status& creature_status) {
	m_consolidated_force = creature_status.felt_needs.dot(m_COEFFICIENTS) + m_OFFSET;
}
