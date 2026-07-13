#ifndef CREATURE7862_H
#define CREATURE7862_H

#include <Eigen/Dense>
#include <random>
#include "SurvivalResource.h"

using Eigen::Vector2d;

class Environment;

//as a survival resource a Creature is a potential mate for other Creatures
class Creature : public SurvivalResource
{
public:
	//if I end up having things like predators and the like
	//tracking species will help me build them from the same base
	enum Species
	{
		gooblet,
		num_species
	};


	enum State
	{
		neutral,
		hungry,
		horny,
		dead,
		num_states
	};

private:
	//some static inline varaibles. These may eventually becomve evolvable
	static inline double m_MAX_HUNGER{ 70.0 };
	static inline double m_REACH{ 5.0 };
	static inline double m_HUNGER_THRESHOLD{ 40.0 };

	const Species m_SPECIES{};
	const double m_SPEED{};
	const double m_EYESIGHT{};

	Vector2d m_position{};

	double m_hunger{ m_HUNGER_THRESHOLD };

	//these track the gooblets current deires.
	//the goal is a pointer toward the survival resource the gooblet is currently after
	//helps prevent us from having to rellook this up constantly.
	SurvivalResource* m_goal{};
	State m_state{ hungry };

	void move_neutral();
	void move_toward(const Vector2d& target);

public:
	Creature( Vector2d position, double speed, double eyesight);

	Creature* reproduce(Creature* other) const;
	void update();
	void print() const;
	void move();
	void eat();

	bool is_dead() const { return m_state == dead; }
	virtual bool is_available() const { return m_state == horny;  }
	Vector2d get_position() const { return m_position; }
	double get_speed() const { return m_SPEED; }
	double get_max_hunger() const { return m_MAX_HUNGER; }
	State get_state() const { return m_state; }
	Species get_Species() const { return m_SPECIES; }

	void set_goal(SurvivalResource* new_goal) { m_goal = new_goal; }
	bool is_seeking() const { return !m_goal || !(m_goal->is_available()); }
	bool goal_in_reach();

	SurvivalResource* get_goal() { return m_goal; }
	//as a Survival resource use means mate
	//This requires a lot of energy so hunger get set to the threshold
	//this will also force the gooblets to not immediately mate again
	virtual void use() { m_hunger = m_HUNGER_THRESHOLD; }
};

#endif