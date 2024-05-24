#pragma once

#include "vector.hpp"

// Axis-aligned bounding box
template<typename float_t>
struct AABBox {
    Vector3<float_t> min;
    Vector3<float_t> max;
};
