#ifndef RAY_HPP_
#define RAY_HPP_

#include "utils/vector.hpp"

class Ray {
public:
    Ray(Vector origin, Vector dir): origin(origin), direction(dir) {}
private:
    Vector origin;
    Vector direction;
};

#endif // RAY_HPP_
