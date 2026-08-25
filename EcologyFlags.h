#ifndef ECOLOGY_FLAG_H80123
#define ECOLOGY_FLAG_H80123

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

#endif
