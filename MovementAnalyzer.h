#ifndef MOVEMENT_DECIDER_H5821
#define MOVEMENT_DECIDER_H5821

#include <Eigen/Dense>
#include "SurvivalResource.h"
#include <vector>
#include "Body.h"
#include <fstream>
#include <string_view>
#include <iostream>
using Eigen::Vector2d;



/*
* An abstract class to enable polymorphism between different ways of processing
* information about the environment and deciding which way to go
*/
class MovementAnalyzer {
public:
	//gathers input information for acceleration into one place
	//this is information about the resource relative to the creatrure
	//stuff that is used in basically every analysis calculation can be precaluclated here
	struct Observation {
		SurvivalResource* object;
		Vector2d displacement;		//displacement from the creature
		double intensity{1.0};		//intensity of the creature's desire
									//so that multiple fruits or competing claims can be accounted for
		double distance{ displacement.norm() };

		void update_displacement(const Vector2d& new_displacement) {
			displacement = new_displacement;
			distance = displacement.norm();
		}

		//instead of updating displacement directly feed it a position to be displaceed from
		void from_perspective(const Vector2d& position) {
			displacement = object->get_nearest(position) - position;
			distance = displacement.norm();
		}
	};

	using NeedVector = Eigen::Matrix<double, static_cast<int>(Body::Need::num_needs), 1>;
	using Status = Body::Status;

	/*
	* Types of movement deciders (see below)
	* This gives a way to do polymorphism for flexibility but still be able to signal back
	* subclass is happening for compatability purposes or being able to build the correct one
	* from some genes
	*/
	enum class AnalyzerType {
		desire,
		friction,
		obstacle,
		num_types
	};
	enum class GeneStructure {
		linear,
		num_gene_types
	};

	struct MetaGenotype {
		AnalyzerType type{};
		GeneStructure structure{};
		MetaGenotype(AnalyzerType _type, GeneStructure _structure) :
			type { _type },
			structure{_structure}
		{};
		virtual MetaGenotype* reproduce(MetaGenotype* mate) const = 0;
		virtual void clear() =0;

		//provides a heading for recording this genotype
		virtual std::string header(std::string_view prefix) const =0;
		//prints this genotype to the given file
		virtual void print_to_file(std::ofstream& file) const = 0;
		virtual std::string as_string() const = 0;
		
		void virtual operator+= (const MetaGenotype* genes)=0;
		friend MetaGenotype* operator+(const MetaGenotype& gene1, const MetaGenotype& gene2);
		friend MetaGenotype* operator/(const MetaGenotype& genes, const double denominator);
		static MetaGenotype* zero(AnalyzerType type);
	};

	struct LinearGenotype : public MetaGenotype {
		NeedVector coefficients{};
		double offset{ 100.0 };
		double exponent{-0.5};

		LinearGenotype(AnalyzerType type, const NeedVector& coefficients, double offset, double exponent);
		MetaGenotype* reproduce(MetaGenotype* mate) const;
		void clear();
		std::string header(std::string_view prefix) const;
		void print_to_file(std::ofstream& file) const;
		std::string as_string() const;

		void operator+=(const MetaGenotype* genes);
		friend LinearGenotype operator+(const LinearGenotype& genes1, const LinearGenotype& genes2);
		friend LinearGenotype operator/(const LinearGenotype& genes, const double denominator);
		static LinearGenotype zero(AnalyzerType type);
	};

	//based on a resource handed in gives a desired acceleration towards or way from the resource
	virtual Vector2d calculate_acceleration(const Observation& observation) const = 0;

	//uses current information to frontload some of the calcuations
	virtual void consolidate(const Status& creature_status) = 0;

	virtual AnalyzerType get_type() const = 0;

	//makes a new movement anlayzer from two initial movement analyzers
	//will yell if they are not of the same type
	virtual MetaGenotype* reproduce(MovementAnalyzer* mate) const;

	virtual MetaGenotype* get_genotype() const = 0;
};

MovementAnalyzer* build_analyzer(const MovementAnalyzer::MetaGenotype* genes);

//some printing helpers
constexpr std::string_view getAnalyzerTypeName(MovementAnalyzer::AnalyzerType type);
std::ostream& operator<<(std::ostream& out, MovementAnalyzer::AnalyzerType type);
std::ofstream& operator<<(std::ofstream& out, MovementAnalyzer::AnalyzerType type);

//adding genotypes (movement analyzer will dynamic cast out then add together
MovementAnalyzer::LinearGenotype operator+(const MovementAnalyzer::LinearGenotype& genes1,
	const MovementAnalyzer::LinearGenotype& genes2);
MovementAnalyzer::MetaGenotype* operator+(const MovementAnalyzer::MetaGenotype& gene1,
	const MovementAnalyzer::MetaGenotype& gene2);

//allow us to devide genotypes by a number.
MovementAnalyzer::MetaGenotype* operator/(const MovementAnalyzer::MetaGenotype& genes,
	const double denominator);
MovementAnalyzer::LinearGenotype operator/(const MovementAnalyzer::LinearGenotype& genes,
	const double denominator);
#endif
