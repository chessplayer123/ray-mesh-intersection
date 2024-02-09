#ifndef TRIANGLE_HPP_
#define TRIANGLE_HPP_

#include <array>
#include "utils/point.hpp"
#include "utils/ray.hpp"

class Triangle {
public:
    Triangle(Point p1, Point p2, Point p3): points({p1, p2, p3}) {}

    Point intersects(const Ray& ray) const;
    Vector get_normal() const;
    bool contains(const Point& point) const;
private:
    std::array<Point, 3> points;
};

#endif // TRIANGLE_HPP_
