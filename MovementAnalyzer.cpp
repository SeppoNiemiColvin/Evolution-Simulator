#include "MovementAnalyzer.h"
#include "ReproHelpers.h"
#include "DesireAnalyzer.h"
#include "FrictionAnalyzer.h"
#include <utility>

MovementAnalyzer* MovementAnalyzer::reproduce(MovementAnalyzer* mate) const {
	//test compatability
	const AnalyzerType mate_type{ mate->get_type() };
	assert(get_type() == mate_type);

	//get the new genome set
	MetaGenotype* my_genes{ get_genotype() };
	MetaGenotype* mate_genes{ mate->get_genotype() };
	MetaGenotype* baby_genes{my_genes->reproduce(mate_genes)};

	//make the new analyzer
	MovementAnalyzer* baby{ build_analyzer(baby_genes) };

	//clean up parent genes.
	delete my_genes;
	delete mate_genes;

	return baby;
}

MovementAnalyzer* build_analyzer(const MovementAnalyzer::MetaGenotype* genes) {
	using LinearGenotype = const MovementAnalyzer::LinearGenotype*;
	MovementAnalyzer* answer{};
	LinearGenotype linear_genes{};
	switch (genes->type) {
	case  MovementAnalyzer::AnalyzerType::desire:
		linear_genes = dynamic_cast<LinearGenotype>(genes) ;
		answer = new DesireAnalyzer{ linear_genes };
		break;
	case MovementAnalyzer::AnalyzerType::friction:
		linear_genes = dynamic_cast<LinearGenotype>(genes);
		answer = new FrictionAnalyzer{ linear_genes };
		break;
	}
	return answer;
}

MovementAnalyzer::LinearGenotype::LinearGenotype(AnalyzerType _type,
	const NeedVector& _coefficients, double _offset, double _exponent):
	MetaGenotype{_type},
	coefficients{_coefficients},
	offset{_offset},
	exponent{_exponent}
{}

MovementAnalyzer::MetaGenotype* MovementAnalyzer::LinearGenotype::reproduce(MovementAnalyzer::MetaGenotype* mate) const {
	if (mate->type != type) {
		throw std::pair<AnalyzerType, AnalyzerType>{type, mate->type};
	}
	LinearGenotype* mate_genes = dynamic_cast<LinearGenotype*>(mate);

	NeedVector coefficients{};
	for (int need{}; need < Body::Need::num_needs; ++need) {
		coefficients[need] = combine_flat(coefficients[need], mate_genes->coefficients[need]);
	}
	return new LinearGenotype{ type, coefficients, combine_flat(offset, mate_genes->offset),
		combine_flat(exponent, mate_genes->exponent, 5e-3) };
}