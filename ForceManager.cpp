#include "ForceManager.h"



ForceManager::ForceManager(const Flags& flags) :
	m_flags{flags},
	m_brain(flags.size())
{ 
}



ForceManager::ForceManager(const Genotype& genes) :
	m_flags{ genes.make_flags()},
	m_brain(m_flags.size()),
	m_relationships{genes.relationships}
{
	for (int i{}; i < m_flags.size(); ++i) {
		for (const auto& sub_genes : genes.genes.at(m_flags[i])) {
			m_brain[i].push_back(build_analyzer(sub_genes));
		}
	}
}

ForceManager::Genotype ForceManager::reproduce( const ForceManager& mate) const {
	//check compatability
	assert(get_flags() == mate.get_flags());

	Genotype baby{m_relationships};

	//build up the baby from its parts asserting that they are compatable as they go
	for (int flag_pos{}; flag_pos < m_flags.size(); ++flag_pos) {
		const std::vector<MovementAnalyzer*>& mate_analyzers{ mate.m_brain[flag_pos]};
		const int length{static_cast<int>( m_brain[flag_pos].size())};
		Ecology flag{ m_flags[flag_pos] };
		assert(length == mate_analyzers.size());
		for (int analyzer_pos{}; analyzer_pos < length; ++analyzer_pos) {
			//assert(m_brain[flag_pos][analyzer_pos]->get_type() == mate_analyzers[analyzer_pos]->get_type());
			baby.genes[flag].push_back(m_brain[flag_pos][analyzer_pos]->reproduce(mate_analyzers[analyzer_pos]));
		}
	}
	return baby;
}

Vector2d ForceManager::response(const MovementAnalyzer::Status& creature_status, const View& surroundings){
	assert(surroundings.size() == m_flags.size() && "View not compatible with creature");
	consolidate(creature_status);

 	Vector2d acceleration{};
	for (int ecology_type{}; ecology_type < surroundings.size(); ++ecology_type) {
		//if the view has a flag this creature doesn't use something probably needs debugging
		//but we can also just choose to skip it
		
		//this coefficient will shrink as we move through the observations to reduce the chance of indecisiveness
		double distance_adjustment{ 1.0 };
		for (const Observation& observation : surroundings[ecology_type]) {
			acceleration += distance_adjustment * observation.intensity * run_observation(ecology_type, observation);
			distance_adjustment *= 0.6;
		}
	}
	return acceleration;
}

ForceManager::Genotype ForceManager::get_genotype() const {
	Genotype genes{ m_relationships};
	for (int eco_flag{}; eco_flag < m_brain.size(); ++eco_flag) {
		std::vector<MovementAnalyzer::MetaGenotype*> sub_genes{};
		const std::vector<MovementAnalyzer*>& eco_analyzers{ m_brain[eco_flag] };
		for (int analyzer{}; analyzer < eco_analyzers.size(); ++analyzer) {
			sub_genes.push_back(eco_analyzers[analyzer]->get_genotype());
		}
		genes.genes[m_flags[eco_flag]] = sub_genes;
	}
	return genes;
}

void ForceManager::consolidate(const MovementAnalyzer::Status& creature_status) {
	for (auto& eco_analyzers : m_brain) {
		for (auto& analyzer : eco_analyzers) {
			analyzer->consolidate(creature_status);
		}
	}
}

Vector2d ForceManager::run_observation(int ecology_pos, const Observation& observation) {
	Vector2d observation_impact{};
	for (const auto& analyzer : m_brain[ecology_pos]) {
		observation_impact += analyzer->calculate_acceleration(observation);
	}
	return observation_impact;
}

ForceManager::~ForceManager() {
	for (auto& eco_responder : m_brain) {
		for (auto analyzer : eco_responder) {
			delete analyzer;
		}
	}
}

void ForceManager::Genotype::build_resource_flag(Ecology flag, MetaEcology type) {
	using LinearGenotype = MovementAnalyzer::LinearGenotype;

	//set the coefficients based on the meta-ecology
	MovementAnalyzer::NeedVector coefficients{};
	double offset{ 100.0 };
	switch(type) {
	case MetaEcology::food:
		coefficients(Body::Need::calories) = -120.0;
		break;
	case MetaEcology::water:
		coefficients(Body::Need::water) = -120.0;
		break;
	case MetaEcology::mate:
		constexpr double gen_force{ 200.0 };
		for (int need{}; need< Body::Need::num_needs; ++need) {
			coefficients(need) = gen_force/2;
		}
		offset = -.4 * gen_force;
	}
	
	//add the corresponding pieces
	genes[flag].push_back(new LinearGenotype{ MovementAnalyzer::AnalyzerType::desire,
		coefficients, offset, -0.3
		});
	constexpr double friction_adjustment{ 0.4 };
	genes[flag].push_back(new LinearGenotype{ MovementAnalyzer::AnalyzerType::friction,
		friction_adjustment * coefficients, friction_adjustment * offset, -0.8
		});

	relationships[flag] = type;
}

ForceManager::Genotype::~Genotype() {
	for (auto& ecotype : genes) {
		for (int i{}; i < ecotype.second.size(); ++i) {
			MovementAnalyzer::MetaGenotype* pointer =  ecotype.second[i];
			delete pointer;
		}
	}
}

std::string ForceManager::Genotype::header() const {
	std::string answer{};
	for (auto& flag : genes) {
		for (auto& metagene : flag.second) {
			std::string prefix{};
			prefix += getEcoName(flag.first);
			prefix += "_";
			prefix += getAnalyzerTypeName(metagene->type);
			answer += metagene->header(prefix);
		}
	}
	return answer;
}

std::string ForceManager::Genotype::as_string() const {
	std::string answer{};
	for (auto& flag : genes) {
		for (auto& metagene : flag.second) {
			answer += metagene->as_string();
		}
	}
	return answer;
}

void ForceManager::Genotype::print_to_file(std::ofstream& file) const {
	file << as_string();
}

MetaEcology ForceManager::get_relationship(Ecology flag) const {
	if (m_relationships.contains(flag)) {
		return m_relationships.at(flag);
	}

	//default flag that should signal that this resource isn't relevant
	return MetaEcology::num_metaflags;
}

namespace DefaultBrainGenes {
	ForceManager::Genotype gooblet() {
		ForceManager::Genotype genes{};
		genes.build_resource_flag(Ecology::bush, MetaEcology::food);
		genes.build_resource_flag(Ecology::pond, MetaEcology::water);
		genes.build_resource_flag(Ecology::gooblet_mate, MetaEcology::mate);
		return genes;
	}

	ForceManager::Genotype gooblet_zero() {
		using MetaGenotype = MovementAnalyzer::MetaGenotype;
		using AnalyzerType = MovementAnalyzer::AnalyzerType;
		ForceManager::Genotype genes{};
		MetaGenotype* new_genes{};
		new_genes = MetaGenotype::zero(AnalyzerType::desire);
		genes.add_analyzer(Ecology::bush, new_genes);
		genes.add_analyzer(Ecology::pond, new_genes);
		genes.add_analyzer(Ecology::gooblet_mate, new_genes);
		delete new_genes;
		new_genes = MetaGenotype::zero(AnalyzerType::friction);
		genes.add_analyzer(Ecology::bush, new_genes);
		genes.add_analyzer(Ecology::pond, new_genes);
		genes.add_analyzer(Ecology::gooblet_mate, new_genes);
		delete new_genes;

		genes.relationships[Ecology::bush] = MetaEcology::food;
		genes.relationships[Ecology::pond] = MetaEcology::water;
		genes.relationships[Ecology::gooblet_mate] = MetaEcology::mate;
		return genes;
	}
}

using Genotype = ForceManager::Genotype;
Genotype operator+(const Genotype& genes1, const Genotype& genes2) {
	//check to make sure the structure is compatable
	if (genes1.relationships != genes2.relationships) {
		throw "relationships don't match";
	}

	Genotype answer{genes1.relationships};

	//build up the genes
	for (auto iter : genes1.genes) {
		if (iter.second.size() != genes2.genes.at(iter.first).size()) {
			throw "metagene size does not match";
		}
		answer.genes[iter.first] = std::vector<ForceManager::MetaGenotype*>{};
		std::vector<ForceManager::MetaGenotype*>& new_line{ answer.genes[iter.first] };
		const int length{ static_cast<int>(iter.second.size()) };
		for (int i{}; i < length; ++i) {
			new_line.push_back(*iter.second[i] + *genes2.genes.at(iter.first)[i]);
		}
	}

	return answer;
}

Genotype operator/(const Genotype& genes, const double denominator) {
	Genotype answer{ genes.relationships };

	//build up the genes
	for (auto iter : genes.genes) {

		answer.genes[iter.first] = std::vector<ForceManager::MetaGenotype*>{};
		std::vector<ForceManager::MetaGenotype*>& new_line{ answer.genes[iter.first] };
		const int length{ static_cast<int>(iter.second.size()) };
		for (int i{}; i < length; ++i) {
			new_line.push_back(*iter.second[i] / denominator);
		}
	}

	return answer;
}

ForceManager::Genotype ForceManager::Genotype::zero(Species structure) {
	switch (structure) {
	case Species::gooblet:
		return DefaultBrainGenes::gooblet_zero();
	}
}

void ForceManager::Genotype::operator+=(const ForceManager::Genotype& summand) {
	for (auto iter : genes) {
		assert(summand.genes.contains(iter.first));
		assert(summand.genes.at(iter.first).size() == iter.second.size()); 
		const std::vector<MetaGenotype*>& analyzers_other = summand.genes.at(iter.first);
		for (int i{}; i < iter.second.size(); ++i) {
			*iter.second[i] += analyzers_other[i];
		}
	}
}

void ForceManager::Genotype::clear() {
	for (auto iter : genes) {
		for (auto& metagene : iter.second) {
			metagene->clear();
		}
	}
}

std::vector<Ecology> ForceManager::Genotype::make_flags() const {
	std::vector<Ecology> answer{};
	for (auto iter : genes) {
		answer.push_back(iter.first);
	}
	return answer;
}