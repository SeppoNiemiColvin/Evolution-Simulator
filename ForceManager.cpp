#include "ForceManager.h"



ForceManager::ForceManager(const Flags& flags) :
	m_flags{flags},
	m_brain(flags.size())
{ 
}


ForceManager::ForceManager(const Genotype& genes) :
	m_flags{ genes.flag_list},
	m_brain(genes.flag_list.size())
{
	for (int i{}; i < genes.flag_list.size(); ++i) {
		for (const auto& sub_genes : genes.genes.at(m_flags[i])) {
			m_brain[i].push_back(build_analyzer(sub_genes));
		}
	}
}

ForceManager ForceManager::reproduce( const ForceManager& mate) const {
	//check compatability
	assert(get_flags() == mate.get_flags());

	ForceManager baby{m_flags};

	//build up the baby from its parts asserting that they are compatable as they go
	for (int flag_pos{}; flag_pos < m_flags.size(); ++flag_pos) {
		const std::vector<MovementAnalyzer*>& mate_analyzers{ mate.m_brain[flag_pos]};
		const int length{static_cast<int>( m_brain[flag_pos].size())};
		assert(length == mate_analyzers.size());
		for (int analyzer_pos{}; analyzer_pos < length; ++analyzer_pos) {
			assert(m_brain[flag_pos][analyzer_pos]->get_type() == mate_analyzers[analyzer_pos]->get_type());
			baby.m_brain[flag_pos].push_back(m_brain[flag_pos][analyzer_pos]->reproduce(mate_analyzers[analyzer_pos]));
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
			distance_adjustment *= 0.7;
		}
	}
	return acceleration;
}

ForceManager::Genotype ForceManager::get_genotype() const {
	Genotype genes{ m_flags };
	for (int eco_flag{}; eco_flag < m_brain.size(); ++eco_flag) {
		std::vector<MovementAnalyzer::MetaGenotype*> sub_genes{};
		const std::vector<MovementAnalyzer*>& eco_analyzers{ m_brain[eco_flag] };
		for (int analyzer{}; analyzer < eco_analyzers.size(); ++analyzer) {
			sub_genes.push_back(eco_analyzers[analyzer]->get_genotype());
		}
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
		constexpr double gen_force{ 140.0 };
		for (int need{}; need< Body::Need::num_needs; ++need) {
			coefficients(need) = gen_force/2;
		}
		offset = -.6 * gen_force;
	}
	
	//add the corresponding pieces
	genes[flag].push_back(new LinearGenotype{ MovementAnalyzer::AnalyzerType::desire,
		coefficients, offset, -0.3
		});
	constexpr double friction_adjustment{ 0.15 };
	genes[flag].push_back(new LinearGenotype{ MovementAnalyzer::AnalyzerType::friction,
		friction_adjustment * coefficients, friction_adjustment * offset, -0.9
		});
	//see if we need to add this to the flag list
	for (auto stored_flag : flag_list) {
		if (stored_flag == flag) {
			return;
		}
	}
	flag_list.push_back(flag);
}

ForceManager::Genotype::~Genotype() {
	for (auto& ecotype : genes) {
		for (auto analyzer_genes : ecotype.second) {
			delete analyzer_genes;
		}
	}
}