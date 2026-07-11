#ifndef CREATURE7862_H
#define CREATURE7862_H

#include <Eigen/Dense>
#include <random>
#include "SurvivalResource.h"

using Eigen::Vector2d;

class Environment;

class Creature : SurvivalResource
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
	static inline double m_MAX_HUNGER{ 50.0 };
	static inline double m_REACH{ 5.0 };
	static inline double m_HUNGER_THRESHOLD{ 30.0 };

	const Species m_SPECIES{};
	const double m_SPEED{};

	Vector2d m_position{};
	double m_hunger{ 15.0 };
	State m_state{ hungry };


	void move_neutral();
	void move_toward_food();

public:
	Creature( Vector2d position, double speed);

	void move_toward(const Vector2d& target);

	Creature reproduce(const Creature mate) const;
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

	virtual void use() { m_hunger = m_HUNGER_THRESHOLD; }
};

#endif