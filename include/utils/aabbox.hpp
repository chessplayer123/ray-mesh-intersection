#pragma once

#include "utils/vector.hpp"

// Axis-aligned bounding box
struct AABBox {
    AABBox(Vector min, Vector max): min(min), max(max) {
    }

    Vector min;
    Vector max;
};
