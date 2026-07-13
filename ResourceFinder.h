#ifndef RESOURCEMANAGER7563
#define RESOURCEMANAGER7563


#include<Eigen/Dense>
#include<unordered_set>
#include <array>

using Eigen::Vector2d;

//This Manages SurvivalResources of fixed length.
//T should be a SurvivalResource
//U should point to a container of SurvivalResources of the same type

template <typename T>
class ResourceFinder
{
private:
	std::vector<T> m_all_resources{};
	std::unordered_set<T*> m_available_resources{};

public:
	ResourceFinder(std::vector<T> all_resources) : m_all_resources{ all_resources }
	{
		update();
	}

	ResourceFinder() {
		;
	}

	//Finds the available resource positioned closest to position
	T* find_closest_to(Vector2d position) {
		T* closest{};
		double current_distance{};

		for (auto resource : m_available_resources) {
			const double distance{ resource->get_distance(position)};
			if (!closest || distance < current_distance) {
				current_distance = distance;
				closest = resource;
			}
		}
		return closest;
	}

	//updates the availability lists
	void update() {
		for (T& resource : m_all_resources) {
			resource.update();
			if (resource.is_available()) {
				m_available_resources.insert( &resource );
			}
			else {
				m_available_resources.erase(&resource);
			}
		}
	}


	void print() const {
		for (auto& resource : m_all_resources) {
			resource.print();
		}
	}

	void extend(std::vector<T> extension) {
		for (T resource : extension) {
			m_all_resources.push_back(resource);
		}
		update();
	}
};

#endif
