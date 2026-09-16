#ifndef TESTING_SUITE84103
#define TESTING_SUITE84103
#include <string>
/*
* This is a header for a file containing various functions I would repeatedly
* run while testing out my simulations. A lot of these were originally in main
* and are meant to be edited directly, but this allows me to easily swap out
* overarching strategies or flip back and forth between things as I work
*/


/*
* creates a single creature and sees if it can survive in a simulation
* bound gives how long to wait for it to die
* it returns when it died or bound if it did not die
*/

int test_survivability(int bound, double food_distance, double water_density);

/*
* creates a single creature that wanders around a map,
*printing out its location as it goes. Useful to test
*/
void wander(const std::string& file_name, int bound, bool verbose = false);

/*
* simulates a small population so you can get more fine tuned but
* general feedback. Created to explore mysterious population crashes
*/

void small_pop(const std::string& file_name, int bound);

/*
* Given an average distancce between food sources
* and a variable indicating the size of the map,
* runs the simulation for 1e4 ticks and prints
* population statistics 
* to file_path at each tick
*/
void test_density(double map_size, double food_diameter, const std::string& file_path);

//makes sure region intersection mechanics are working properly
bool test_regions();

//makes sure the resource finder is working well
bool test_resource_finder();


//make sure the creatures are working reasaonably
bool test_creatures();

//makes sure the resource set is working well
bool test_resource_set();

//some functions to test that the movement analyzers are working as intended
bool test_desire_analyzer(bool verbose = false);

bool test_friction_analyzer(bool verbose = false);

bool test_force_manager(bool verbose = false);
#endif