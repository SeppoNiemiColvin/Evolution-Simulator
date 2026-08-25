#ifndef RESOURCE_ANALYZER_H9310
#define RESOURCE_ANALYZER_H9310
#include "MovementAnalyzer.h"
#include <Eigen/Dense>
#include <cmath>

class DesireAnalyzer :
    public MovementAnalyzer
{
private:
    const NeedVector m_COEFFICIENTS{};
    const double m_OFFSET{};
    const double m_EXPONENT{ -0.3 };

    //once forces have been consolidated down put here
    double m_consolidated_force{};


public:
    DesireAnalyzer(const LinearGenotype* genes);

    AnalyzerType get_type() const { return AnalyzerType::desire; }

    MetaGenotype* get_genotype() const;

    Vector2d calculate_acceleration(const Observation& observation) const;

    void consolidate(const Status& creature_status);

};

#endif