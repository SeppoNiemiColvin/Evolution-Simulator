#ifndef GENOME_H017839
#define GENOME_H017839
#include "Creature.h"
#include <unordered_map>
#include "EcologyFlags.h"
#include <utility>
#include <iostream>
#include <fstream>
#include <string>

/*
* Tracks the average genotype of the population
* Separate from genome so that non numerical genes can be tallied in other ways
* i.e. a gene that is bool will need to be converted to an int
*/
class PopulationGenome
{
private:
	const Species m_type{};
	Creature::Genotype m_gene_total{ Creature::Genotype::zero(m_type) };
	int m_population_size{};

	//place to track some qualitative data that can evolve
	//such as if we include more classical genes with dominant/rescessive
public:
	PopulationGenome(Species type) : m_type{ type } {
	}

	PopulationGenome(const Creature::Genotype& genes) : m_gene_total{ genes / 1.0 },
		m_population_size{1}
	{
	}

	Species get_type() const { return m_type; }
	//gets the average genotype seen
	Creature::Genotype average_genotype() const {
		if (m_population_size == 0) {
			return m_gene_total / 1.0; //this is to ensure a copy is made and the result is not destructed
		}
		else {
			return m_gene_total / m_population_size;
		}
	}

	int get_population_size() const { return m_population_size; }

	void clear() {
		m_gene_total.clear();
 		m_population_size = 0;
	}

	//produces the header for recording the genome
	std::string header() const;

	//add in new genes,populatrion
	void operator+=(const PopulationGenome& pop);
	void operator+=(const Creature::Genotype& genes);
	void operator+=(const Creature* animal);

	friend std::ostream& operator<<(std::ostream& out, const PopulationGenome& pop);
};


std::ostream& operator<<(std::ostream& out, const PopulationGenome& pop);

std::ofstream& operator<<(std::ofstream& out, const PopulationGenome& pop);
#endif