#ifndef FORCE_MANAGER_H6824
#define FORCE_MANAGER_H6824

#include "MovementAnalyzer.h"
#include "EcologyFlags.h"
#include <unordered_map>
#include <unordered_set>
#include <Eigen/Dense>
#include <vector>
#include <utility>
#include <fstream>

using Eigen::Vector2d;

/*
* This collects the MovementAnalyzers together to give a comprehensive acceleration
*/

class ForceManager
{
public:
	using Observation = MovementAnalyzer::Observation;
	using View = std::vector<std::vector<Observation>>;
	using Surroundings = std::unordered_map < Ecology, std::vector<SurvivalResource*>>;
	using Flags = std::vector<Ecology>;
	using MetaGenotype = MovementAnalyzer::MetaGenotype;
	using StructureGenes = std::unordered_map<Ecology, std::vector<MetaGenotype*>>;
	
	struct Genotype {
		std::unordered_map<Ecology, MetaEcology> relationships{};
		StructureGenes genes{};


		//adds a specific analyzer's genes to a particular flag
		void add_analyzer(Ecology flag, MetaGenotype* sub_genes) {
			//we need to copy that data so that the Genotype has a copy that it owns
			MetaGenotype* copied{};
			switch (sub_genes->structure) {
			case MovementAnalyzer::GeneStructure::linear:
				copied = new MovementAnalyzer::LinearGenotype{
					dynamic_cast<MovementAnalyzer::LinearGenotype&>(*sub_genes) };
				break;
			}
			genes[flag].push_back(copied);
		}

		//basic analyzer set up that keys in flag as a potential food source
		//and thus to move towards it if calories are low
		void build_resource_flag(Ecology flag, MetaEcology type);

		std::vector<Ecology> make_flags() const;

		Genotype() {};
		Genotype( std::unordered_map<Ecology, MetaEcology> _relationships) :
			relationships{ _relationships }
		{};

		//prints a header for this genotype
		std::string header() const;
		//prints the information contained in this genotype to the file
		void print_to_file(std::ofstream& file) const;
		std::string as_string() const;

		void operator+=(const Genotype& genes);
		friend Genotype operator+(const Genotype& genes1, const Genotype& genes2);
		friend Genotype operator/(const Genotype& genes, const double denominator);

		static Genotype zero(Species structure);
		void clear();
		~Genotype();
	};

private:
	//the flags provide the keys for brain
	const Flags m_flags;
	std::vector<std::vector<MovementAnalyzer*>> m_brain{};
	std::unordered_map<Ecology, MetaEcology> m_relationships{};
	//passes creature_status to each MovementAnalyzer to make sure each is consolidated
	void consolidate(const Body::Status& creature_status);
	Vector2d run_observation(int ecology_pos, const Observation& observation);

public:
	ForceManager(const Flags& flags);
	ForceManager(const Genotype& genes);
	~ForceManager();

	Flags get_flags() const {
		return m_flags;
	}
	//calculates a desired acceleration based on feedback from the surroundings and the creature's current status
	//note that this assumes the view vectors are sorted based on distance from the creature
	Vector2d response(const Body::Status& creature_status, const View& surroundings);


	//create a new force manager based on 
	ForceManager::Genotype reproduce(const ForceManager& mate) const;

	//get the genotype for this item
	Genotype get_genotype() const;

	MetaEcology get_relationship(Ecology flag) const;
};

namespace DefaultBrainGenes {
	//bush is food, pond is water, gooblet_mate is mate
	ForceManager::Genotype gooblet();
	ForceManager::Genotype gooblet_zero();
}

//adding genotypes
ForceManager::Genotype operator+(const ForceManager::Genotype& genes1,
	const ForceManager::Genotype& genes2);
ForceManager::Genotype operator/(const ForceManager::Genotype& genes,
	const double denominator);
#endif