#ifndef RAY_HPP_
#define RAY_HPP_

#include "vector.hpp"

class Ray {
public:
    Ray(Point origin, Vector dir): origin(origin), direction(dir) {}
private:
    Point origin;
    Vector direction;
};

#endif // RAY_HPP_
