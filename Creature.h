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

using Eigen::Vector2d;

class Environment;

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

	//by modifying this list we can more easily add in new features to track
	//to make the creature's needs more complex
	enum Physical_Need
	{
		calories,
		water,
		num_needs
	};

	//current state of the gooblet.
	//note the first states correspond to being low on the corresponding needs
	//
	enum State
	{
		hungry,
		thirsty,
		horny,
		dead,
		num_states
	};
private:
	const Species m_SPECIES{}; 

	//reserve caps determine how much of that need the creature can store.
	//water is lower to reflect that we need to drink more frequently/can't last as long without water as food
	const std::array<double,num_needs> m_RESERVE_CAPS{100.0, 75.0};
	std::array<double,num_needs> m_current_reserves{ m_RESERVE_CAPS };

	// these thresholds affect how the creature transitions between states
	// providing the fraction of the reserve cap that must be met for matting can happen
	//they are static to provide a species wide constraint if the caps evolve.
	static inline std::array<double,num_needs> m_THRESHOLDS{ 0.6, 0.5 };

	
	//movement properties. Evolvable traits
	const double m_MAX_SPEED{};
	const double m_EYESIGHT{};
	const double m_MAX_ACCELERATION{5.0};
	//movement/action property. Not currently evolvable
	static inline double m_REACH{ 5.0 };

	//to track current movement/position info
	Vector2d m_position{};
	Vector2d m_velocity{};
	//m_acceleration will reset to 0 each update, but has its own place here
	//to help multiple processes build it up to the currect value each tick
	Vector2d m_acceleration{};

	//these track the gooblets current deires.
	//the goal is a pointer toward the survival resource the gooblet is currently after
	//helps prevent us from having to rellook this up constantly.
	SurvivalResource* m_goal{};
	State m_state{ State::hungry };
	 
	//these are some helper functions to set the acceleration
	//will be replaced when I move away from singular goal/state bsed behavior
	//provides an acceleration when there is no particular destination
	void acc_neutral();
	//provides an acceleration based around a particular target destination
	void acc_toward(const Vector2d& target);



	//helper function to test if we need to change state
	bool above_threshold(Physical_Need need);

public:
	Creature( Vector2d position, double speed, double eyesight);
	Creature(Vector2d position, double speed, double accelearation, double eyesight);
	void print() const;

	Creature* reproduce(Creature* other) const;

	//basic functions that move the creature through based on chosen actions
	void update();

	void move();
	void eat();

	//getters describing the creature
	bool is_dead() const { return m_state == State::dead; }
	virtual bool is_available() const { return m_state == State::horny;  }
	Vector2d get_position() const { return m_position; }
	double get_max_speed() const { return m_MAX_SPEED; }
	double get_acceleration() const { return m_MAX_ACCELERATION; }
	double get_max_energy() const { return m_RESERVE_CAPS[Physical_Need::calories]; }
	State get_state() const { return m_state; }
	Species get_Species() const { return m_SPECIES; }
	double get_eyesight() const { return m_EYESIGHT; }

	//goal tracking/control
	void set_goal(SurvivalResource* new_goal);
	bool is_seeking() const { return !m_goal || !(m_goal->is_available()); }
	bool goal_in_reach();
	SurvivalResource* get_goal() { return m_goal; }

	//as a Survival resource use means mate
	//This requires a lot of energy so hunger get set to the threshold
	//this will also force the gooblets to not immediately mate again
	virtual void use() {
		m_current_reserves[calories] = m_RESERVE_CAPS[calories] * m_THRESHOLDS[calories];
}

	//some friend functions that mainly exist to keep the creature in bounds
	//and eventually add obstacles. They are friensd to allow them to modify the 
	friend void HalfPlane::push_out(Creature* creature) const;
	friend void Region::pull_inside(Creature* creature) const;
	friend void Rectangle::push_out(Creature* creatue) const;
};

std::ostream& operator<<(std::ostream& out, Creature::State state);

#endif