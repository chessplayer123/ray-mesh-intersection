#pragma once

#include <vector>
#include "vector.hpp"
#include "ray.hpp"
#include "kd_tree.hpp"


std::vector<Vector> parallel_intersects(
    const Ray& ray,
    const KDTree& tree,
    int threads_count,
    double epsilon = std::numeric_limits<double>::epsilon()
);
