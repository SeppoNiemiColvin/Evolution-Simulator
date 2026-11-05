#ifndef BODY_H95167
#define BODY_H95167

#include <array>
#include <Eigen/Dense>
#include "Region.h"
#include "HalfPlane.h"
#include "Rectangle.h"
#include <string>
#include <string_view>
#include <unordered_set>
#include <fstream>
#include <iostream>

/*
* A class to help  refactor the creature class into more managable parts
* This handles the impact of various physical needs and exertion but does not make any
* desicions for the creature
*/

using Eigen::Vector2d;

class MovementAnalyzer;
class Creature;

class Body
{
public:
	//by modifying this list we can more easily add in new features to track
	//to make the creature's needs more complex without overwhelming the creature class
	enum Need
	{
		water,
		calories,
		num_needs
	};

	using NeedSet = std::unordered_set<Need>;
	using NeedList = std::array<double, num_needs>;
	using NeedVector = Eigen::Matrix<double, static_cast<int>(num_needs), 1>;

	//these mark different locations in the genotype to more easily signal where a problem occured
	//when  testing validity. only variables that can go wrong are listed
	enum class Gene {
		reserve_cap,
		max_speed,
		max_acceleration,
		starting_amount
	};

	//bundles the needed information to make a creature together for easier passing to construction
	struct Genotype {
		//largest amount that the creature could store for each need
		NeedVector reserve_caps{ 200.0, 150.0 };
		//fastest the creature can go
		double max_speed{ 100.0 };
		//largest acceleration the creture can apply
		double max_acceleration{ max_speed / 2 };
		//any needs we plan on ignoring
		NeedSet ignored_needs{};
		//amount of each need the creature starts with when born
		NeedVector starting_amounts{ .7 * reserve_caps[water], 0.5 * reserve_caps[calories] };


		//makes sure this genotype produces something viable.
		// Raises an exception if not
		void test_validity() const;
		//sets the numerical genes to 0 (so it is the additive identity)
		void clear();

		//printing
		std::string static header();
		void print_to_file(std::ofstream& file) const;
		std::string to_string() const;

		void operator+=(const Genotype& genes);
		friend Genotype operator+(const Genotype& genes1, const Genotype& genes2);
		friend Genotype operator/(const Genotype& genes, double denominator);
		//makes a base genotype that has nothing in it. Designed as an addative identity
		static Genotype zero();
	};

	//gathers input information for consolidate into one place
	//this is stuff about the creature itself/the creature's expereince of the world
	//may eventually include things like temperature
	struct Status {
		NeedVector felt_needs;
		const Vector2d& velocity;
	};

private:
	//reserve caps determine how much of that need the creature can store.
//water is lower to reflect that we need to drink more frequently/can't last as long without water as food
	const NeedVector m_RESERVE_CAPS{200.0, 200.0};

	//controls how fast baseline metabolism happens
	const NeedVector m_METABOLIC_BASE{};
	//a need being included here means it won't be affected at all
	const NeedSet m_IGNORED_NEEDS{};

	NeedVector m_current_reserves{ m_RESERVE_CAPS };
	//movement properties. Evolvable traits
	const double m_MAX_SPEED{};
	const double m_MAX_ACCELERATION{ 5.0 };

	//to track current movement/position info
	Vector2d m_position{};
	Vector2d m_velocity{};

	
public:
	Body(Vector2d position, Genotype genes);

	//given a desired acceleration push the creature in that direction
	//designed to be called as a part of creature's move function after an acceleration
	//has been decided. Handles any metabolic side-effects
	//delta is the time step to use (named to be in parallel with godot's naming)
	void exert(Vector2d& acceleration, double delta = 1);

	//handles the base metabolism of time passing
	void metabolize();

	//get information about the creatures current state
	double current_reserve(Need need) const { return m_current_reserves[need]; }
	std::string print_reserve(Need need) const;
	Status make_status() const;
	double felt_need(Need need) const { return m_current_reserves(need) / m_RESERVE_CAPS(need); };
	Vector2d get_position() const { return m_position; }
	Vector2d get_velocity() const { return m_velocity; }
	double get_speed()  const { return m_velocity.norm(); }
	bool is_dead() const;

	//get information about the creatures genotype
	double get_max_speed() const { return m_MAX_SPEED; }
	double get_max_acceleration() const { return m_MAX_ACCELERATION; }
	double get_max_energy() const { return m_RESERVE_CAPS[Need::calories]; }

	//estimates how long the creature can exist as (living/horny) without finding new resources
	int time_till_unavailable(std::array<double, num_needs>& thresholds) const;
	int time_till_dead() const;

	//action relevant methods
	Genotype get_genes() const;
	Genotype reproduce(Body& mate_body);

	//the versions without input will refill that need completely 
	void eat() {
		m_current_reserves[calories] = m_RESERVE_CAPS[calories];
	}
	void drink() {
		m_current_reserves[water] = m_RESERVE_CAPS[water];
	}
	void eat(double nutrition) {
		m_current_reserves[calories] = fmin(m_current_reserves[calories] + nutrition, m_RESERVE_CAPS[calories]);
	}

	//some friend functions that mainly exist to keep the creature in bounds
//and eventually add obstacles. They are friensd to allow them to modify the 
	friend void HalfPlane::push_out(Creature* creature) const;
	friend void Region::pull_inside(Creature* creature) const;
	friend void Rectangle::push_out(Creature* creatue) const;
};

Body::NeedVector create_metabolic_base(Body::Genotype& genes);

//some functions to help with printing
constexpr std::string_view getNeedName(Body::Need need);
std::ostream& operator<<(std::ostream& out, Body::Need need);
std::ofstream& operator<<(std::ofstream& out, Body::Need need);

constexpr std::string_view getGeneName(Body::Gene gene);
std::ostream& operator<<(std::ostream& out, Body::Gene gene);
std::ofstream& operator<<(std::ofstream& out, Body::Gene gene);

//genotype artithmetic
Body::Genotype operator+(const Body::Genotype& genes1, const Body::Genotype& genes2);
Body::Genotype operator/(const Body::Genotype& genes, double denominator);

std::string to_string(Body::NeedVector needs);
#endif