#include "FrictionAnalyzer.h"


FrictionAnalyzer::FrictionAnalyzer(const LinearGenotype* genes) :
	m_NEED_COEFFICIENTS{ genes->coefficients},
	m_NEED_OFFSET{genes -> offset},
	m_EXPONENT{genes->exponent}
{ }

MovementAnalyzer::MetaGenotype* FrictionAnalyzer::get_genotype() const {
	return  new LinearGenotype{ AnalyzerType::friction ,
		 m_NEED_COEFFICIENTS, m_NEED_OFFSET, m_EXPONENT
	};
}
