#ifndef RESOURCEMANAGER7563
#define RESOURCEMANAGER7563


#include<Eigen/Dense>
#include<unordered_set>
#include <array>

//This Manages SurvivalResources of fixed length.
//T should be a SurvivalResource
//U should point to a container of SurvivalResources of the same type
using Eigen::Vector2d;

template <typename T, auto n>
class ResourceFinder
{
private:
	std::array<T, n> m_all_resources{};
	std::unordered_set<T*> m_available_resources{};

public:
	ResourceFinder(std::array<T,n> all_resources) : m_all_resources{ all_resources }
	{
		update_availability();
	};

	T* find_closest_to(Vector2d position) {
		T* closest{};
		double current_distance{};

		for (auto resource : m_available_resources) {
			const double distance{ (resource->get_position() - position).norm() };
			if (!closest || distance < current_distance) {
				current_distance = distance;
				closest = resource;
			}
		}
		return closest;
	}

	void update_availability() {
		m_available_resources = std::unordered_set<T*>();
		for (T& resource : m_all_resources) {
			if (resource.is_available()) {
				m_available_resources.insert( &resource );
			}
		}
	}
};

#endif
