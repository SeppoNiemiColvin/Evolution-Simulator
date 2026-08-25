#ifndef FORCE_MANAGER_H6824
#define FORCE_MANAGER_H6824

#include "MovementAnalyzer.h"
#include "EcologyFlags.h"
#include <unordered_map>
#include <unordered_set>
#include <Eigen/Dense>
#include <vector>
#include <utility>

using Eigen::Vector2d;


/*
* This collects the MovementAnalyzers together to give a comprehensive acceleration
*/

class ForceManager
{
public:
	using Observation = MovementAnalyzer::Observation;
	using View = std::vector<std::vector<Observation>>;
	using Flags = std::vector<Ecology>;
	using MetaGenotype = MovementAnalyzer::MetaGenotype;
	using StructureGenes = std::unordered_map<Ecology, std::vector<MetaGenotype*>>;

	struct Genotype {
		enum class MetaEcology {
			food,
			water,
			mate
		};

		Flags flag_list{};
		StructureGenes genes{};
		
		//adds a specific analyzer's genes to a particular flag
		void add_analyzer(Ecology flag, MetaGenotype* sub_genes) {
			genes[flag].push_back(sub_genes);
		}

		//basic analyzer set up that keys in flag as a potential food source
		//and thus to move towards it if calories are low
		void build_resource_flag(Ecology flag, MetaEcology type);

		Genotype() { ; }
		Genotype(const Flags& flags) : flag_list{ flags } {};
		~Genotype();
	};

private:
	//the flags should match the keys for brain
	const Flags& m_flags;
	std::vector<std::vector<MovementAnalyzer*>> m_brain{};

	//passes creature_status to each MovementAnalyzer to make sure each is consolidated
	void consolidate(const MovementAnalyzer::Status& creature_status);
	Vector2d run_observation(int ecology_pos, const Observation& observation);

public:
	ForceManager(const Flags& flags);
	ForceManager(const Genotype& genes);

	Flags get_flags() const {
		return m_flags;
	}
	//calculates a desired acceleration based on feedback from the surroundings and the creature's current status
	//note that this assumes the view vectors are sorted based on distance from the creature
	Vector2d response(const MovementAnalyzer::Status& creature_status, const View& surroundings);


	//create a new force manager based on 
	ForceManager reproduce(const ForceManager& mate) const;

	//get the genotype for this item
	Genotype get_genotype() const;
};


#endif