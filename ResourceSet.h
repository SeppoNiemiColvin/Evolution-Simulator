#ifndef RESOURCE_SET_H38910
#define RESOURCE_SET_H38910
#include <unordered_set>
#include <unordered_map>
#include <Eigen/Dense>
#include <utility>
#include <vector>
#include "Prehash.h"
#include "SurvivalResource.h"
#include "MovementAnalyzer.h"
#include <iostream>
/*
* This is similar to ResourceFinder but designed for creatures
* so that the overall population can grow and shrink as creatures are born or
* die off. This means the owner variable will be a set instead of a vector
* and even the owner is managing pointers
* Kept as a template so that if other resources grow/shrink/move die
* They can use this as well
*/

using Eigen::Vector2d;

template<typename T>
class ResourceSet
{
public:
	using LocationLookUp = std::unordered_map<CoordPrehash, std::vector<T*>>;
	using Report = std::pair<T*, double>; //this records the closest resource and distance
	using Observation = MovementAnalyzer::Observation;

private:
	std::unordered_set<T*> m_all_resources{};
	LocationLookUp m_available_resources{};
	double m_block_size{ 130.0 };

	CoordPrehash prehash(const Vector2d& position) const {
		return hash::prehash(position, m_block_size);
	}

	std::unordered_set<CoordPrehash> relevant_coords(const Vector2d& position, double eyesight) const {
		return std::unordered_set<CoordPrehash>{
			prehash(position),
			prehash(position + eyesight * Vector2d(0, 1)),
			prehash(position - eyesight * Vector2d(0, 1)),
			prehash(position + eyesight * Vector2d(1, 0)),
			prehash(position - eyesight * Vector2d(1, 0)),
			prehash(position + eyesight * Vector2d(1, 1)),
			prehash(position - eyesight * Vector2d(1, 1)),
			prehash(position + eyesight * Vector2d(1, -1)),
			prehash(position - eyesight * Vector2d(1, -1))
		};
	}

	Report check_coords(const Vector2d& position, double eyesight, CoordPrehash coord_to_check) {
		//handle no coords
		if (!m_available_resources.contains(coord_to_check) || m_available_resources[coord_to_check].empty()) {
			return Report{ nullptr, eyesight };
		}
		T* closest{};
		double current_distance{ eyesight };

		for (auto resource : m_available_resources[coord_to_check]) {
			if (!(resource->is_available())) {
				continue;
			}
			else {
				const double distance{ resource->get_distance(position) };
				if (!closest || distance < current_distance) {
					current_distance = distance;
					closest = resource;
				}
			}
		}
		return Report{ closest, current_distance };
	}

	//adds an resource to the corresponding coord(s) iff it is available
	void place_resource(T* resource) {
		if (resource->is_available()) {
			std::vector<CoordPrehash> coords{ resource->prehash(m_block_size) };
			for (const auto coord : coords) {
				m_available_resources[coord].push_back(resource);
			}
		}
	}

public:
	//finds the available resource closest to position
	T* find_closest_to(const Vector2d& position, double eyesight) {
		T* closest{};
		double distance{eyesight};

		std::unordered_set<CoordPrehash> coords_to_check{ relevant_coords(position, eyesight) };
		for (auto coord : coords_to_check) {
			Report report{ check_coords(position, distance, coord) };
			if (report.second < distance) {
				closest = report.first;
				distance = report.second;
			}
		}
		return closest;
	}

	std::vector<T*> within_eyesight(const Vector2d& position, double eyesight) {
		std::vector<T*> answer{};
		std::unordered_set<T*> seen{};
		double distance{ eyesight };
		
		std::unordered_set<CoordPrehash> coords_to_check{ relevant_coords(position, eyesight) };

		for (auto coord : coords_to_check) {
			if (!m_available_resources.contains(coord) || m_available_resources[coord].empty()) {
				continue;
			}
			for (const auto& resource : m_available_resources[coord]) {
				if (resource->get_distance(position) <= eyesight) {
					answer.push_back(resource);
				}
			}
		}
		return answer;
	}

	std::vector<Observation> look_around(const Vector2d& position, double eyesight) {
		std::vector<Observation> answer{};
		std::unordered_set<T*> seen{};
		double distance{ eyesight };

		std::unordered_set<CoordPrehash> coords_to_check{ relevant_coords(position, eyesight) };

		for (auto coord : coords_to_check) {
			if (!m_available_resources.contains(coord) || m_available_resources[coord].empty()) {
				continue;
			}
			for (const auto& resource : m_available_resources[coord]) {
				if (resource->get_distance(position) <= eyesight) {
					answer.push_back(Observation{resource});
					answer.back().from_perspective(position);
				}
			}
		}
		return answer;
	}
	//removes a resource from the ResourceSet
	void erase(T* dead_resource) {
		m_all_resources.erase(dead_resource);
		//CoordPrehash coords{ prehash(dead_resource->get_position()) };
		delete dead_resource;
	}




	//updates the resources clearing out those that are dead
	//and shifting availability for the remaining
	void update(const double eyesight) {
		if (eyesight > m_block_size) {
			m_block_size = eyesight + 30.0;
			
		}
		m_available_resources.clear();

		std::vector<T*> graveyard{};
		for (auto& resource : m_all_resources) {

			resource->update();
			if (resource->is_dead()) {

				graveyard.push_back(resource);
			}

			place_resource(resource);
		}

		for (auto& skeleton : graveyard) {
			erase(skeleton);
		}
	}

	//adds a new resource to the pile
	void insert(T* new_resource) {
		m_all_resources.insert(new_resource);
		if (new_resource->is_available()) {
			CoordPrehash coord{ prehash(new_resource->get_position()) };
			m_available_resources[coord].push_back(new_resource);
		}
	}

	//iterator sat the start of the resources
	std::unordered_set<T*>::iterator begin() const {
		return m_all_resources.begin();
	}

	//iterator for the end of the resources
	std::unordered_set<T*>::iterator end() const {
		return m_all_resources.end();
	}

	//destructor that cleans up the resources
	~ResourceSet() {
		for (T* resource : m_all_resources) {
			delete resource;
		}
	}

	//number of total resources
	int size() const { return m_all_resources.size(); }

	//number of resources that are actually available
	int available_size() const {
		int total{};
		for (auto coord : m_available_resources) {
			total += static_cast<int>(coord.second.size());
		}
		return total;
	}
};

#endif