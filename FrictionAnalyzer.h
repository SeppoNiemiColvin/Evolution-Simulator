#pragma once
#include "MovementAnalyzer.h"
#include "Body.h"

/*
* Helps apply friction closer to a resource so that the creature slows down
* to use it
*/


class FrictionAnalyzer :
    public MovementAnalyzer
{
private:
    const NeedVector m_NEED_COEFFICIENTS{};
    const double m_NEED_OFFSET{};
    const double m_EXPONENT{ 0.7 };

    //similarly for the friction info
    Vector2d m_consolidated_velocity{};

public:
    FrictionAnalyzer(const LinearGenotype* genes);
    
    MetaGenotype* get_genotype() const;
    AnalyzerType get_type() const { return AnalyzerType::friction; }

    virtual void consolidate(const Status& creature_status) {
        m_consolidated_velocity = creature_status.velocity * (creature_status.felt_needs.dot(m_NEED_COEFFICIENTS) + m_NEED_OFFSET);
    }

    Vector2d calculate_acceleration(const Observation& observation) const {
        return -fmin(10.0, pow(observation.distance / 5.0, m_EXPONENT)) * m_consolidated_velocity;
    }
};

