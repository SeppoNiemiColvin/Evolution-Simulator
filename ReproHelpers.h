#ifndef REPRO_HELPER_H74101
#define REPRO_HELPER_H74101

//combines two variables in a way that the mutation respects the scale
//useful when we know the variable will be positive
double combine_scaled(const double parent1, const double parent2);

//combines two varaibles and then mutates in a linear fasion irespective of scale
//note scale is on a log scale. useful when we want variables to potentially be negative
double combine_flat(const double parent1, const double parent2, const double scale = 1.0);

//randomly returns true or false
bool coinFlip();
#endif