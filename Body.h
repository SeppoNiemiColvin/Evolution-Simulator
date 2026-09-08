#ifndef BODY_H95167
#define BODY_H95167

#include <array>
#include <Eigen/Dense>
#include "Region.h"
#include "HalfPlane.h"
#include "Rectangle.h"
#include <string>
#include <unordered_set>

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
		NeedList reserve_caps{ 200.0, 200.0 };
		double max_speed{ 100.0 };
		double max_acceleration{ max_speed / 2 };
		NeedSet ignored_needs{};
		NeedList starting_amounts{ reserve_caps[water], 0.5 * reserve_caps[calories] };


		//makes sure this genotype produces something viable.
		// Raises an exception if not
		void test_validity() const;
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
	const NeedList m_RESERVE_CAPS{200.0, 200.0};

	//controls how fast metabolism happens
	const NeedList m_METABOLIC_BASE{ -1, -1 };
	//a need being included here means it won't be affected at all
	const NeedSet m_IGNORED_NEEDS{};

	NeedList m_current_reserves{ m_RESERVE_CAPS };
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
	double felt_need(Need need) const { return m_current_reserves[need] / m_RESERVE_CAPS[need]; };
	Vector2d get_position() const { return m_position; }
	Vector2d get_velocity() const { return m_velocity; }
	double get_speed()  const { return m_velocity.norm(); }
	bool is_dead() const;

	//get information about the creatures genotype
	double get_max_speed() const { return m_MAX_SPEED; }
	double get_max_acceleration() const { return m_MAX_ACCELERATION; }
	double get_max_energy() const { return m_RESERVE_CAPS[Need::calories]; }

	//gets how long the creature can exist as (living/horny) without finding new resources
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

Body::NeedList create_metabolic_base(Body::Genotype& genes);

#endif