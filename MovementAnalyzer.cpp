#include "MovementAnalyzer.h"
#include "ReproHelpers.h"
#include "DesireAnalyzer.h"
#include "FrictionAnalyzer.h"
#include <utility>

MovementAnalyzer::MetaGenotype* MovementAnalyzer::reproduce(MovementAnalyzer* mate) const {
	//test compatability
	const AnalyzerType mate_type{ mate->get_type() };
	assert(get_type() == mate_type);

	//get the new genome set
	MetaGenotype* my_genes{ get_genotype() };
	MetaGenotype* mate_genes{ mate->get_genotype() };
	MetaGenotype* baby_genes{my_genes->reproduce(mate_genes)};

	//clean up parent genes.
	delete my_genes;
	delete mate_genes;

	return baby_genes;
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
	MetaGenotype{_type, GeneStructure::linear},
	coefficients{_coefficients},
	offset{_offset},
	exponent{_exponent}
{}

MovementAnalyzer::MetaGenotype* MovementAnalyzer::LinearGenotype::reproduce(MovementAnalyzer::MetaGenotype* mate) const {
	if (mate->type != type) {
		throw std::pair<AnalyzerType, AnalyzerType>{type, mate->type};
	}
	LinearGenotype* mate_genes = dynamic_cast<LinearGenotype*>(mate);

	NeedVector baby_coefficients{};
	for (int need{}; need < Body::Need::num_needs; ++need) {
		baby_coefficients[need] = combine_flat(coefficients[need], mate_genes->coefficients[need], 3.0);
	}
	return new LinearGenotype{ type, baby_coefficients, combine_flat(offset, mate_genes->offset, 3.0),
		combine_flat(exponent, mate_genes->exponent, 3e-2) };
}


std::string MovementAnalyzer::LinearGenotype::header(std::string_view prefix) const {
	std::string answer{};
	for (int need{}; need < Body::Need::num_needs; ++need) {
		answer += prefix;
		answer += '_';
		answer += getNeedName(static_cast<Body::Need>(need));
		answer += "_coefficient,";
	}
	answer += prefix;
	answer += "_offset,";
	answer += prefix;
	answer += "_exponent,";
	return answer;
}

std::string MovementAnalyzer::LinearGenotype::as_string() const {
	std::string answer{};
	answer += to_string(coefficients);

	answer += std::to_string( offset) + ',';
	answer  += std::to_string(exponent) + ',';
	return answer;
}

void MovementAnalyzer::LinearGenotype::print_to_file(std::ofstream& file) const {
	file << as_string();
}

constexpr std::string_view getAnalyzerTypeName(MovementAnalyzer::AnalyzerType type) {
	using Type = MovementAnalyzer::AnalyzerType;
	switch (type) {
	case Type::desire:		return "desire";
	case Type::friction:	return "friction";
	case Type::obstacle:	return "obstacle";
	case Type::num_types:	return "analyzerType";
	}
}

std::ostream& operator<<(std::ostream& out, MovementAnalyzer::AnalyzerType type) {
	out << getAnalyzerTypeName(type);
	return out;
}
std::ofstream& operator<<(std::ofstream& out, MovementAnalyzer::AnalyzerType type) {
	out << getAnalyzerTypeName(type);
	return out;

}

using MetaGenotype = MovementAnalyzer::MetaGenotype;
using LinearGenotype = MovementAnalyzer::LinearGenotype;
MetaGenotype* operator+(const MetaGenotype& gene1, const MetaGenotype& gene2) {
	if (gene1.structure != gene2.structure) {
		throw "incorrect structure";
	}
	if (gene1.type != gene2.type) {
		throw "incorrect structure";
	}
	
	switch (gene1.structure) {
	case MovementAnalyzer::GeneStructure::linear:
		const LinearGenotype& linear_genes1{ dynamic_cast<const LinearGenotype&>(gene1) };
		const LinearGenotype& linear_genes2{ dynamic_cast<const LinearGenotype&>(gene2) };
		return new LinearGenotype{ linear_genes1 + linear_genes2 };
	}
}



void LinearGenotype::operator+=(const MetaGenotype* genes) {
	assert(genes->structure == MovementAnalyzer::GeneStructure::linear);
	assert(genes->type == type);

	const LinearGenotype* summand{ dynamic_cast<const LinearGenotype*>(genes) };
	coefficients += summand->coefficients;
	offset += summand->offset;
	exponent += summand->exponent;
}

LinearGenotype operator+(const LinearGenotype& genes1, const LinearGenotype& genes2){
	return LinearGenotype{ genes1.type, genes1.coefficients + genes2.coefficients,
		genes2.offset +genes1.offset, genes1.exponent + genes1.exponent
	};
}

MetaGenotype* operator/(const MetaGenotype& genes, const double denominator) {
	switch (genes.structure) {
	case MovementAnalyzer::GeneStructure::linear:
		const LinearGenotype& linear_genes{ dynamic_cast<const LinearGenotype&>(genes) };
		return new LinearGenotype{ linear_genes / denominator };
	}
}

LinearGenotype operator/(const LinearGenotype& genes, const double denominator) {
	return LinearGenotype{ genes.type, genes.coefficients / denominator,
		genes.offset / denominator, genes.exponent / denominator
	};
}

MetaGenotype* MetaGenotype::zero(AnalyzerType type) {
	GeneStructure structure{};
	switch (type) {
	case AnalyzerType::desire:
		structure = GeneStructure::linear;
		break;

	case AnalyzerType::friction:
		structure = GeneStructure::linear;
		break;
	}

	switch (structure) {
	case GeneStructure::linear:
		return new LinearGenotype{ LinearGenotype::zero(type) };
	}
}
LinearGenotype LinearGenotype::zero(AnalyzerType type) {
	return LinearGenotype(type, NeedVector{}, 0.0, 0.0);
}

void LinearGenotype::clear() {
	coefficients = NeedVector{};
	offset = 0.0;
	exponent = 0.0;
}