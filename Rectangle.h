#ifndef RECTANGLE_H9321
#define RECTANGLE_H9321

#include "Region.h"
#include <Eigen/Dense>

class Creature;

using Eigen::Vector2d;

class Rectangle :
    public Region
{
private:
    double m_left{};
    double m_right{};
    double m_lower{1.0};
    double m_upper{1.0};
public:
    Rectangle();
    Rectangle(double left, double right, double lower, double upper);
    double get_left() const { return m_left; }
    double get_right() const { return m_right; }
    double get_lower() const { return m_lower; }
    double get_upper() const { return m_right; }
    double get_area() const { return (m_right - m_left) * (m_upper - m_lower); }
    Vector2d get_nearest(const Vector2d& position) const;
    void push_out(Creature* creature) const;
    bool is_inside(const Vector2d& position) const;
    Vector2d random_point() const;
};

#endif

