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
#include "ForceManager.h"
#include <unordered_map>

using Eigen::Vector2d;

//as a survival resource a Creature is a potential mate for other Creatures
class Creature : public SurvivalResource
{
public:
	using Surroundings = std::unordered_map<Ecology, std::vector<SurvivalResource*>>;
	using NeedList = Body::NeedList;
	using Observation = MovementAnalyzer::Observation;

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
		ForceManager::Genotype brain_genes{};
		//raises an exception if there is a problem with this genotype
		void test_validity() const;
	};

	//bundles an observation with its type so that way we have enough information to interpret the goal
	struct Goal {
		Observation desire;
		MetaEcology type;

		double get_distance() const { return desire.distance; }
		void from_perspective(const Vector2d& position) { desire.from_perspective(position); }
		bool is_available() const { return desire.object->is_available(); }
		Vector2d get_position() const { return desire.object->get_position(); }
	};

private:
	const Species m_SPECIES{}; 
	Body m_body;

	// these thresholds affect how the creature transitions between states
	// providing the fraction of the reserve cap that must be met for matting can happen
	//they are static to provide a species wide constraint if the caps evolve.
	static inline NeedList s_MATING_THRESHOLDS{ 0.6, 0.6 };
	static inline NeedList s_SEEKING_THRESHOLDS{ 0.8, 0.8 };

	const double m_EYESIGHT{};
	//movement/action property. Not currently evolvable
	static inline double s_REACH{ 5.0 };

	//m_acceleration will reset to 0 each update, but has its own place here
	//to help multiple processes build it up to the currect value each tick
	Vector2d m_acceleration{};

	//these track the gooblets current deires.
	//the goal is a pointer toward the survival resource the gooblet is currently after
	//helps prevent us from having to rellook this up constantly.
	Goal* m_goal{};
	State m_state{ State::hungry };
	ForceManager m_brain;


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
	void acc_sight(const ForceManager::View& surroundings);


	//helper function to test if we need to change state
	bool above_threshold(Body::Need need);

	//these tasks help deligate keeping the view information in order
	//before sending it to the force manager
	void manage_view(ForceManager::View& surroundings) const ;
	//this makes the veiew and in the process updates the goal
	ForceManager::View make_view(const Surroundings& surroundings);
	//updates goal based on a survival resource with an ecology flag
	void update_goal(Ecology flag, const Observation& potential_goal);
	//decides whether to repalce a given goal
	bool should_replace_goal(Body::Need need, double distance) const;

public:
	Creature(const Vector2d& position, const Genotype& genes);
	Creature(const Vector2d& position, const Genotype& genes, const ForceManager& brain);
	void print() const;

	Creature* reproduce(Creature* other);

	//basic functions that move the creature through based on chosen actions
	void update();

	void move(const Surroundings& surroundings = { {} });
 
	//pass these onto the body
	void eat() { m_body.eat(); }
	void drink() { m_body.drink(); }
	void eat(double nutrition) { m_body.eat(nutrition); }

	//getters describing the creature
	bool is_dead() const { return m_state == State::dead; }
	virtual bool is_available() const {
		for (int need{}; need < Body::num_needs; ++need) {
			if (s_MATING_THRESHOLDS[need] > m_body.felt_need(static_cast<Body::Need>(need))) {
				return false;
			}
		}
		return true;
	}
	Vector2d get_position() const { return m_body.get_position(); }
	Vector2d get_velocity() const { return m_body.get_velocity(); }
	double get_max_speed() const { return m_body.get_max_speed(); }
	double get_max_acceleration() const { return m_body.get_max_acceleration(); }
	double get_max_energy() const { return m_body.get_max_energy(); }
	double get_current_reserve(Body::Need need) const { return m_body.current_reserve(need); }
	State get_state() const { return m_state; }
	Species get_species() const { return m_SPECIES; }
	double get_eyesight() const { return m_EYESIGHT; }
	double get_reach() const { return s_REACH; }

	//goal tracking/control
	std::vector<Ecology> get_flags() const {
		return m_brain.get_flags();
	}

	bool is_seeking() const { return !m_goal || !(m_goal->is_available()); }
	bool goal_in_reach();
	Goal* get_goal() { return m_goal; }
	void set_goal(SurvivalResource* goal, MetaEcology type) {
		delete m_goal;
		m_goal = new Goal{ Observation{goal}, type };
		m_goal->from_perspective(get_position());
	}

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