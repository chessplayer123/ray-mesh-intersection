#include "ray.hpp"


bool Ray::intersects(const AABBox& box) const {
    Vector t1 = (box.min - origin) * inv_vector;
    Vector t2 = (box.max - origin) * inv_vector;

    double tmin = std::min(t1.x(), t2.x());
    double tmax = std::max(t1.x(), t2.x());

    tmin = std::max(tmin, std::min(t1.y(), t2.y()));
    tmax = std::min(tmax, std::max(t1.y(), t2.y()));

    tmin = std::max(tmin, std::min(t1.z(), t2.z()));
    tmax = std::min(tmax, std::max(t1.z(), t2.z()));

    return tmax >= 0 && tmin <= tmax;
}


