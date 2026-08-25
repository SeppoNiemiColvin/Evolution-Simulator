#ifndef CREATURE7862_H
#define CREATURE7862_H

#include <Eigen/Dense>
#include <random>
#include <iostream>
#include <array>
#include "SurvivalResource.h"
#include "Region.h"
#include "HalfPlane.h"
#include "Rectangle.h"
#include <utility>
#include "Body.h"

using Eigen::Vector2d;

//as a survival resource a Creature is a potential mate for other Creatures
class Creature : public SurvivalResource
{
public:
	//if I end up having things like predators and the like
	//tracking species will help me build them from the same base
	//While having something to mark that certain creatures shouldn't
	//even try to reproduce
	enum Species
	{
		gooblet,
		num_species
	};

	//current state of the gooblet.
	//note the first states correspond to being low on the corresponding needs
	//
	enum State
	{
		thirsty,
		hungry,
		horny,
		dead,
		num_states
	};

	//enum where I can put variables from the genotype that can go wrong
	//to make from easier debugging
	enum class Gene {
		eyesight
	};
	struct Genotype
	{
		Body::Genotype body_genes{};
		double eyesight{ body_genes.max_speed * 3 };
		bool is_female{ true };

		//raises an exception if there is a problem with this genotype
		void test_validity() const;
	};
private:
	const Species m_SPECIES{}; 
	bool m_is_female{ true };
	Body m_body;

	// these thresholds affect how the creature transitions between states
	// providing the fraction of the reserve cap that must be met for matting can happen
	//they are static to provide a species wide constraint if the caps evolve.
	static inline std::array<double,Body::Need::num_needs> m_THRESHOLDS{ 0.6, 0.7 };

	const double m_EYESIGHT{};
	//movement/action property. Not currently evolvable
	static inline double m_REACH{ 5.0 };

	//m_acceleration will reset to 0 each update, but has its own place here
	//to help multiple processes build it up to the currect value each tick
	Vector2d m_acceleration{};

	//these track the gooblets current deires.
	//the goal is a pointer toward the survival resource the gooblet is currently after
	//helps prevent us from having to rellook this up constantly.
	SurvivalResource* m_goal{};
	State m_state{ State::hungry };

	//These were put in to help debug population crashes to help see what the issue was
	//may eventually inforporate death from old age
	int m_age{};
	int m_time_since_mated{ -300 };
	 
	//these are some helper functions to set the acceleration
	//will be replaced when I move away from singular goal/state bsed behavior
	//provides an acceleration when there is no particular destination
	void acc_neutral();
	//provides an acceleration based around a particular target destination
	void acc_toward(const Vector2d& target);



	//helper function to test if we need to change state
	bool above_threshold(Body::Need need);

public:
	Creature(const Vector2d& position, const Genotype& genes);
	void print() const;

	Creature* reproduce(Creature* other);

	//basic functions that move the creature through based on chosen actions
	void update();

	void move();

	//pass these onto the body
	void eat() { m_body.eat(); }
	void drink() { m_body.drink(); }
	void eat(double nutrition) { m_body.eat(nutrition); }

	//getters describing the creature
	bool is_dead() const { return m_state == State::dead; }
	virtual bool is_available() const { return m_state == State::horny;  }
	Vector2d get_position() const { return m_body.get_position(); }
	Vector2d get_velocity() const { return m_body.get_velocity(); }
	double get_max_speed() const { return m_body.get_max_speed(); }
	double get_max_acceleration() const { return m_body.get_max_acceleration(); }
	double get_max_energy() const { return m_body.get_max_energy(); }
	double get_current_reserve(Body::Need need) const { return m_body.current_reserve(need); }
	State get_state() const { return m_state; }
	Species get_species() const { return m_SPECIES; }
	double get_eyesight() const { return m_EYESIGHT; }
	bool is_female() const { return m_is_female; }
	double get_reach() const { return m_REACH; }
	//goal tracking/control
	void set_goal(SurvivalResource* new_goal);
	bool is_seeking() const { return !m_goal || !(m_goal->is_available()); }
	bool goal_in_reach();
	SurvivalResource* get_goal() { return m_goal; }

	//as a Survival resource use means mate
	//This requires a lot of energy so hunger get set to the threshold
	//this will also force the gooblets to not immediately mate again
	virtual void use() {
		goal_in_reach();
		if(!above_threshold(Body::Need::calories)) {
			m_state = hungry;
		}
		m_time_since_mated = 0;
	}

	//gets how long the creature can exist as (living/horny) without finding new resources
	int time_till_unavailable() const;
	int time_till_dead() const;
	//some friend functions that mainly exist to keep the creature in bounds
	//and eventually add obstacles. They are friensd to allow them to modify the 
	friend void HalfPlane::push_out(Creature* creature) const;
	friend void Region::pull_inside(Creature* creature) const;
	friend void Rectangle::push_out(Creature* creatue) const;
};

std::ostream& operator<<(std::ostream& out, Creature::State state);

#endif