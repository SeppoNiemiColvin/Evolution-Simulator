#include "testingsuite.h"
#include "Simulation.h"

/*
* Sees how long a single creature can survive on its own up to some bound.
* I.e. if the creature survives until bound units, we'll stop and just return
* bound, but if the creature dies earlier we return the time when that happened.
*/
int test_survivability(int bound) {
	Simulation simulation{ 0.0, 300.0, 0.0, 300.0, 5.0, 150.0, 1, 1 , false };
	for (int time{}; time < bound; ++time) {
		simulation.update();

		if (simulation.population_size() == 0) {
			return time;
		}
	}

	return bound;
}

void wander(int bound) {
	Simulation simulation{ 0.0, 300.0, 0.0, 300.0, 10.0, 100.0, 1, 1, false };
	for (int time{}; time < bound; ++time) {
		simulation.update();
		std::cout << "Time: " << time << '\n';
		std::cout << '\n' << '\n';
		if (simulation.population_size() == 0) {
			return;
		}
	}
}