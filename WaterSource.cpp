#include "WaterSource.h"

//prints out information about this pond
void WaterSource::print() const{
	std::cout << "Pond " << m_pond.get_left() << " <= x <= " << m_pond.get_right();
	std::cout << '\t';
	std::cout << " " << m_pond.get_lower() << " <= y <= " << m_pond.get_upper();
	std::cout << '\n';
}



//hashes out the region this pond is in
std::vector<CoordPrehash> WaterSource::prehash(double block_size) const {
	double x_coord{ m_pond.get_left() };
	const double x_coord_end{ m_pond.get_right() };
	const double y_coord_base{ m_pond.get_lower() };
	const double y_coord_end{ m_pond.get_upper() };
	double y_coord{ y_coord_base };

	std::vector<CoordPrehash> answer{};
	while (x_coord <= x_coord_end && y_coord <= y_coord_end) {
		//get these coordinates
		
		answer.push_back(hash2D::prehash(x_coord, y_coord, block_size));

		//incriment point
		y_coord += block_size;
		if (y_coord > y_coord_end) {
			y_coord = y_coord_base;
			x_coord += block_size;
		}
	}

	const bool x_same_block{hash2D::same_block(x_coord - block_size, x_coord_end, block_size)};


	if (!x_same_block) {
		y_coord = y_coord_base;
		while (y_coord <= y_coord_end) {
			answer.push_back(hash2D::prehash(x_coord_end, y_coord, block_size));
			y_coord += block_size;
		}
	}

	y_coord = y_coord_base + block_size * static_cast<CoordPrehash>(m_pond.get_height() / block_size);

	const bool y_same_block{ hash2D::same_block(y_coord, y_coord_end, block_size) };
	if (!y_same_block){
		x_coord = m_pond.get_left();
		while (x_coord <= x_coord_end) {
			answer.push_back(hash2D::prehash(x_coord, y_coord_end, block_size));
			x_coord += block_size;
		}
	}

	if (!x_same_block && !y_same_block) {
		answer.push_back(hash2D::prehash(x_coord_end, y_coord_end, block_size));
	}
	return answer;
}


bool overlap(std::vector<WaterSource>& current_water, Rectangle new_pond) {
	for (const auto& pond : current_water) {
		if (pond.m_pond.intersects(new_pond)) {
			return true;
		}
	}
	return false;
}

void generate_ponds(const Rectangle& map_base, double margin, double density, std::vector<WaterSource>& out){
	std::mt19937_64 generator;
	//see what has already been put there to adjust the area

	double remaining_area{ map_base.get_area() * density };
	for (const auto& pond : out) {
		const double height{ pond.m_pond.get_height() + 70 };
		const double width{ pond.m_pond.get_width() + 70 };
		remaining_area -= width * height;
	}

	const int num_pond{ static_cast<int>(remaining_area / 2.89e4) };
	std::uniform_real_distribution horizontal(map_base.get_left(), map_base.get_right() - 170);
	std::uniform_real_distribution vertical(map_base.get_lower(), map_base.get_right() - 170);

	for (int i{}; i < num_pond; ++i) {
		Rectangle pond_withMargin{ 0, 170, 0, 170 };
		Vector2d point(horizontal(generator), vertical(generator));
		while (overlap(out, pond_withMargin + point)) {
			point = Vector2d(horizontal(generator), vertical(generator));
		}
		out.push_back(WaterSource{ Rectangle{ 70,100, 70, 100 } + point });
	}
}