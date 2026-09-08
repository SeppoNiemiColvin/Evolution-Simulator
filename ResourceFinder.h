#ifndef RESOURCEMANAGER7563
#define RESOURCEMANAGER7563


#include<Eigen/Dense>
#include<unordered_set>
#include<vector>
#include<unordered_map>
#include<utility>
#include "Prehash.h"

using Eigen::Vector2d;
class SurvivalResource;

//This Manages SurvivalResources of fixed length.
//T should be a SurvivalResource
//U should point to a container of SurvivalResources of the same type

template <typename T>
class ResourceFinder
{
public:
	//data structure that will make look up within eyessight range easier
	using LocationLookUp = std::unordered_map<CoordPrehash, std::unordered_set<T*>>;
	using Report = std::pair<T*, double>; //this records the closest resource and its distance

private:
	//this is the owner of the resources
	std::vector<T> m_all_resources{};

	LocationLookUp m_available{};
	//how big should we make the blocks for our look up helpers
	double m_block_size{ 130.0 };

	/*
	* Member functions:
	*/
	//converts the vector into a more reasonable thing to hash
	// which groups positions into block size chunks
	CoordPrehash prehash(const Vector2d& position) const {
		return hash::prehash(position, m_block_size);
	}

	Report check_coords(const Vector2d& position, double eyesight,const CoordPrehash coord_to_check){
		//handle no coords
		if (!m_available.contains(coord_to_check) || m_available[coord_to_check].empty()) {
			return Report{ nullptr, eyesight };
		}
		T* closest{};
		double current_distance{eyesight};

		std::vector<T*> clean_up{};

		for (auto resource : m_available[coord_to_check]) {
			if (!(resource ->is_available())) {
				clean_up.push_back(resource);
				continue;
			}
			const double distance{ resource->get_distance(position) };
			if (!closest || distance < current_distance) {
				current_distance = distance;
				closest = resource;
			}
		}
		for (auto resource : clean_up) {
			m_available[coord_to_check].erase(resource);
		}
		return Report{ closest, current_distance };
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

	void add_available(T& resource) {
		std::vector<CoordPrehash> coords{ resource.prehash(m_block_size) };

		//need to make sure this resource is findable
		for (const auto coord : coords) {
			m_available[coord].insert(&resource);
		}
	}

	void remove_unavailable(T& resource) {
		std::vector<CoordPrehash> coords{ resource.prehash(m_block_size) };

		for (const auto coord : coords) {
			m_available[coord].erase(&resource);
			if (m_available[coord].empty()) {
				m_available.erase(coord);
			}
		}

	}

public:
	ResourceFinder(std::vector<T>& all_resources, double  eyesight) :
		m_all_resources{ all_resources },
		m_block_size{ eyesight }
	{
		update(eyesight);
	}

	ResourceFinder(double eyesight) : m_block_size{ eyesight } {
		;
	}


	//makes sure individual resources are updated and arranges the resources into a dictionary
	//that makes it easier to test which resources are within eyesight
	void update(double max_eyesight) {
		if (max_eyesight > m_block_size) {
			m_block_size = max_eyesight + 30.0;
			m_available.clear();
		}

		for (T& resource : m_all_resources) {
			resource.update();

			if (resource.is_available()) {
				add_available(resource);

			}
			else {
				remove_unavailable(resource);
			}
		}
	}

	//finds the resource closet to position within eyesight constraint
	T* find_closest_to(const Vector2d& position, double eyesight) {
		//if the eyesight is bigger than the block size, reupdate with that
		if (eyesight > m_block_size) {
			m_block_size = eyesight + 30;
			update(eyesight);
		}
		//get what coordinates we need to check
		std::unordered_set<CoordPrehash> coords_to_check{
			relevant_coords(position, eyesight)
		};

		T* closest{};
		double distance{ eyesight };
		for (auto coord : coords_to_check) {
			Report report{ check_coords(position, distance, coord) };
			if (report.second < distance) {
				closest = report.first;
				distance = report.second;
			}
		}
		return closest;
	}
	//provides a vector of all available T within eyesight of position
	std::vector<T*> within_eyesight(const Vector2d& position, double eyesight) {
		std::unordered_set<T*> seen{};
		std::vector<T*> answer{};
		std::unordered_set<CoordPrehash> coords_to_check{
			relevant_coords(position,eyesight)
		};
		for (auto coord : coords_to_check) {
			if (!m_available.contains(coord) || m_available[coord].empty()) {
				continue;
			}
			for (const auto resource : m_available[coord]) {
				if (resource->get_distance(position) <= eyesight && !seen.contains(resource)) {
					answer.push_back( (resource));
					seen.insert(resource);

				}
			}
			
		}

		return answer;
	}

	std::vector<SurvivalResource*> within_eyesight_generic(const Vector2d& position, double eyesight) {
		std::unordered_set<T*> seen{};
		std::vector<SurvivalResource*> answer{};
		std::unordered_set<CoordPrehash> coords_to_check{
			relevant_coords(position,eyesight)
		};
		for (auto coord : coords_to_check) {
			if (!m_available.contains(coord) || m_available[coord].empty()) {
				continue;
			}
			for (const auto resource : m_available[coord]) {
				if (resource->get_distance(position) <= eyesight && !seen.contains(resource)) {
					answer.push_back(static_cast<SurvivalResource*>(resource));
					seen.insert(resource);

				}
			}

		}

		return answer;
	}

	//prints all the resources in this container
	void print() const {
		for (auto& resource : m_all_resources) {
			resource.print();
		}
	}

	//add the resources in extension to this resource finder
	void extend(std::vector<T>& extension) {
		for (T resource : extension) {
			m_all_resources.push_back(resource);
		}
		update(m_block_size);
	}

	std::vector<T>::const_iterator cbegin() const {
		return m_all_resources.cbegin();
	}

	std::vector<T>::const_iterator cend() const {
		return m_all_resources.cend();
	}

	std::vector<T>::iterator begin() {
		return m_all_resources.begin();
	}

	std::vector<T>::iterator end() {
		return m_all_resources.end();
	}

	int size() const {
		return m_all_resources.size();
	}
};

#endif
