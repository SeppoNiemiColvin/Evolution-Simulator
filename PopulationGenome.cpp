#include "PopulationGenome.h"

std::string PopulationGenome::header() const {
	return m_gene_total.header() + ",Population";
}

void PopulationGenome::operator+=(const PopulationGenome& pop) {
	assert(m_type == pop.m_type);

	m_gene_total += pop.m_gene_total;
	m_population_size += pop.m_population_size;
}

void PopulationGenome::operator+=(const Creature::Genotype& genes) {
	m_gene_total += genes;
	++m_population_size;
}

void PopulationGenome::operator+=(const Creature* animal) {
	m_gene_total += animal->get_genotype();
	++m_population_size;
}

std::ostream& operator<<(std::ostream& out, const PopulationGenome& pop) {
	Creature::Genotype genes{ pop.average_genotype()};
	out  << genes.as_string() << ',' << pop.get_population_size();
	return out;
}

std::ofstream& operator<<(std::ofstream& out, const PopulationGenome& pop) {
	Creature::Genotype genes{ pop.average_genotype() };
	out << genes.as_string()  << ',' << pop.get_population_size();
	return out;
}