#ifndef CREATURE7862_H
#define CREATURE7862_H

#include <Eigen/Dense>
#include "Map.h"
#include <random>

using Eigen::Vector2d;

class Creature
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
		hungry,
		horny,
		dead,
		num_states
	};

private:
	const Species m_SPECIES{};
	const double m_SPEED{};
	const double m_MAX_HUNGER{ 50.0 };
	const double m_REACH{ 5.0 };
	Vector2d m_position{};
	double m_hunger{ 15.0 };
	State m_state{ hungry };



	void move_neutral();
	void move_toward_food();

public:
	Creature( Vector2d position, double speed);
	void move_toward(const Vector2d& target);

	Creature reproduce(const Creature mate) const;
	void update_state();
	void print() const;
	void move();

	bool is_dead() const { return m_state == dead; }
	bool is_horny() const { return m_state == horny;  }
	Vector2d get_position() const { return m_position; }
	double get_speed() const { return m_SPEED; }
	double get_max_hunger() const { return m_MAX_HUNGER; }
	State get_state() const { return m_state; }
	Species get_Species() const { return m_SPECIES; }
	void eat() { m_hunger = m_MAX_HUNGER; }
};

#endif