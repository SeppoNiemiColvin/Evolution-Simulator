#ifndef WATERSOURCE_H_4092
#define WATERSOURCE_H_4092

#include "SurvivalResource.h"
#include "Rectangle.h"
#include <iostream>
#include <vector>

class Creature;

class WaterSource :
    public SurvivalResource
{
private:
    const Rectangle m_pond;

public:
    WaterSource(double left, double right, double lower, double upper) :
        m_pond{Rectangle{left, right, lower, upper}}
    {
        ;
    }

    WaterSource(Rectangle pond) : m_pond { pond } {
        ;
    }

    Vector2d get_position() const { return m_pond.random_point(); }
    Vector2d get_nearest(Vector2d position) const { return m_pond.get_nearest(position); }

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

    friend bool overlap(std::vector<WaterSource>& current_water, Rectangle new_pond);
};

bool overlap(std::vector<WaterSource>& current_water, Rectangle new_pond);
#endif

