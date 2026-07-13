#ifndef RESOURCE_SET_H38910
#define RESOURCE_SET_H38910
#include <unordered_set>
#include <Eigen/Dense>

/*
* This is similar to ResourceFinder but designed so that
* the resource can grow or shrink over time
*/

using Eigen::Vector2d;

template<typename T>
class ResourceSet
{
private:
	std::unordered_set<T*> m_all_resources{};
	std::unordered_set<T*> m_available_resources{};
public:
	//finds the available resource closest to position
	T* find_closest_to(Vector2d position) {
		T* closest{};
		double current_distance{};
		for (T* resource : m_available_resources) {
			const double distance{ resource -> get_distance(position)};
			if (!closest || distance < current_distance) {
				current_distance = distance;
				closest = resource;
			}
		}
		return closest;
	}

	//removes a resource from the ResourceSet
	void erase(T* dead_resource) {
		m_all_resources.erase(dead_resource);
		if (dead_resource->is_available()) {
			m_available_resources.erase(dead_resource);
		}
		delete dead_resource;
	}

	//updates the resources clearing out those that are dead
	//and shifting availability for the remaining
	void update() {
		for (auto& resource : m_all_resources) {
			resource->update();
			if (resource->is_dead()) {
				erase(resource);
				break;
			}

			if (resource->is_available()) {
				m_available_resources.insert(resource);
			}
			else {
				m_available_resources.erase(resource);
			}
		}
	}

	//adds a new resource to the pile
	void insert(T* new_resource) {
		m_all_resources.insert(new_resource);
		if (new_resource->is_available()) {
			m_available_resources.insert(new_resource);
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
	int available_size() const { return m_available_resources.size(); }
};

#endif