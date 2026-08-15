
#include <random>
#include <cmath>

/*
* A function to provide the averaging and mutation aspects from parents
* in a way that respects scale. Note that this works best when the property in question
* is assumed to be positive
*/
double combine_scaled(const double parent1, const double parent2) {
	static std::mt19937_64 generator;
	static std::normal_distribution distribution(0.0, 1.0);

	const double average{ (parent1 + parent2) / 2 };
	return average * pow(1.03, distribution(generator));
}

double combine_flat(const double parent1, const double parent2, const double scale) {
	const double average{ (parent1 + parent2) / 2 };
	static std::mt19937_64 generator;
	static std::normal_distribution distribution(0.0, 1.0);

	return average + pow(2.8, scale) * distribution(generator);
}

bool coinFlip() {
	static std::mt19937_64 generator;
	return generator() % 2 == 0;
}