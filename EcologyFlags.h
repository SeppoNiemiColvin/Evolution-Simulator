#ifndef ECOLOGY_FLAG_H80123
#define ECOLOGY_FLAG_H80123

#include <iostream>
#include <fstream>
/*
* This enumeration will help creatures communicate with the simulation as to
* what information it is looking for. Defined here since it didn't make sense
* for the simulation or creature to have preferential ownership
*/
enum class Ecology {
	gooblet,
	gooblet_mate,
	bush,
	pond,
	other_obstacle,
	num_flags
};

//these rerepresent the realtionship a creature has to an object
enum class MetaEcology {
	food,
	water,
	mate,
	danger,
	num_metaflags
};

enum class Species
{
	gooblet,
	num_species
};

constexpr std::string_view getEcoName(Ecology flag);
std::ostream& operator<<(std::ostream& out, Ecology flag);
std::ofstream& operator<<(std::ofstream& out, Ecology flag);

constexpr std::string_view getMetaEcoName(MetaEcology flag);
std::ostream& operator<<(std::ostream& out, MetaEcology flag);
std::ofstream& operator<<(std::ofstream& out, MetaEcology flag);
#endif
