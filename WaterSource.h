#ifndef WATERSOURCE_H_4092
#define WATERSOURCE_H_4092

#include "SurvivalResource.h"
#include "Rectangle.h"
#include "Prehash.h"
#include <iostream>
#include <vector>
#include <random>

class Creature;

class WaterSource :
    public SurvivalResource
{
private:
    const Rectangle m_pond;
    const int m_id;

    int set_id() {
        static int id_number{};
        return id_number++;
    }

public:
    WaterSource(double left, double right, double lower, double upper) :
        m_pond{Rectangle{left, right, lower, upper}},
        m_id{set_id()}
    {
        ;
    }

    WaterSource(Rectangle pond) :
        m_pond { pond },
        m_id{ set_id() }
    {
        ;
    }

    Vector2d get_position() const { return m_pond.random_point(); }
    Vector2d get_nearest(Vector2d position) const { return m_pond.get_nearest(position); }
    int get_id() const { return m_id; }
    double get_left() const { return m_pond.get_left(); }
    double get_right() const { return m_pond.get_right(); }
    double get_lower() const { return m_pond.get_lower(); }
    double get_upper() const { return m_pond.get_upper(); }
    //these may shift if I have the water source shrink and grow
    bool is_available() const { return true; }
    void update() { ; }
    void use() { ; }
    void print() const;
    bool is_inside(Vector2d point) const { return m_pond.is_inside(point); }

    void push_out(Creature* creature) const {
        m_pond.push_out(creature);
    }

    bool blocks_sight(const Vector2d start, const Vector2d end) {
        return m_pond.line_intersects(start, end);
    }

    std::vector<CoordPrehash> prehash(double block_size) const;

    friend bool overlap(std::vector<WaterSource>& current_water, Rectangle new_pond);
    friend void generate_ponds(const Rectangle& map_base,
        double margin, double density, std::vector<WaterSource>& out);
};


//does this new rectangle overlap with the previous ponds
bool overlap(std::vector<WaterSource>& current_water, Rectangle new_pond);

//generates a collection of ponds of size 30 x 30 with 70 margins around them
void generate_ponds(const Rectangle& map_base,
    double margin, double density, std::vector<WaterSource>& out);
#endif