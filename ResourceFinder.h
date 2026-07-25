#ifndef RESOURCEMANAGER7563
#define RESOURCEMANAGER7563


#include<Eigen/Dense>
#include<unordered_set>
#include<vector>
#include<unordered_map>
#include<utility>

using Eigen::Vector2d;

//This Manages SurvivalResources of fixed length.
//T should be a SurvivalResource
//U should point to a container of SurvivalResources of the same type

template <typename T>
class ResourceFinder
{
public:
	//we use ints for coordinates so floating point roundng errrors don't affect hashing
	//set as its own type so that way it can be changed and treated as a hash loc rather than an int
	using CoordPrehash = int;
	//data structure that will make look up within eyessight range easier
	using LocationLookUp = std::unordered_map<int, std::unordered_set<T*>>;
	using Report = std::pair<T*, double>; //this records the closest resource and its distance

private:
	//this is the owner of the resources
	std::vector<T> m_all_resources{};

	LocationLookUp m_available{};
	//how big should we make the blocks for our look up helpers
	double m_block_size{ 100.0 };

	/*
	* Member functions:
	*/
	//converts the vector into a more reasonable thing to hash
	// which groups positions into block size chunks
	CoordPrehash prehash(const Vector2d position) {
		const int x_coord{static_cast<int>( position(0) / m_block_size) };
		const int y_coord{ static_cast<int>(position(1) / m_block_size) };
		return (x_coord << 6) | y_coord;
	}

	Report check_coords(Vector2d position, double eyesight, CoordPrehash coords_to_check) {
		//handle no coords
		if (!m_available.contains(coords_to_check) or m_available[coords_to_check].empty()) {
			return Report{ nullptr, eyesight };
		}
		T* closest{};
		double current_distance{eyesight};

		for (auto resource : m_available[coords_to_check]) {
			const double distance{ resource->get_distance(position) };
			if (!closest || distance < current_distance) {
				current_distance = distance;
				closest = resource;
			}
		}
		return Report{ closest, current_distance };
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
			m_block_size = max_eyesight + 20;
			m_available.clear();
		}

		for (T& resource : m_all_resources) {
			resource.update();
			//this will likely need ot be adjusted once we get resources that are regions
			CoordPrehash coords{ prehash(resource.get_position()) };
			if (resource.is_available()) {
				//need to make sure this resource is findable
				m_available[coords].insert(&resource);
			}
			else if (m_available.contains(coords)){
				m_available[coords].erase(&resource);
				if (m_available[coords].empty()) {
					m_available.erase(coords);
				}
			}
		}
	}

	//finds the resource closet to position within eyesight constraint
	T* find_closest_to(Vector2d position, double eyesight) {
		//if the eyesight is bigger than the block size, reupdate with that
		if (eyesight > m_block_size) {
			update(eyesight);
		}

		//get what coordinates we need to check
		std::unordered_set<CoordPrehash> coords_to_check{
			prehash(position),
			prehash(position + eyesight * Vector2d(0,1)),
			prehash(position - eyesight * Vector2d(0,1)),
			prehash(position + eyesight * Vector2d(1,0)),
			prehash(position - eyesight * Vector2d(1,0)),
			prehash(position + eyesight * Vector2d(1,1)),
			prehash(position - eyesight * Vector2d(1,1)),
			prehash(position + eyesight * Vector2d(1,-1)),
			prehash(position - eyesight * Vector2d(1,-1))
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
};

#endif
