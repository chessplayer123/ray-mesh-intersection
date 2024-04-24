#pragma once

#include <vector>
#include <cstddef>
#include <vector>
#include "utils/box.hpp"
#include "utils/triangle.hpp"

class TriangularMesh {
public:
    TriangularMesh(): triangles() {}

    TriangularMesh(std::vector<Triangle>& data): triangles(data) {}

    TriangularMesh(std::vector<Triangle>&& data): triangles(std::move(data)) {}

    inline size_t size() const {
        return triangles.size();
    }

    inline const Triangle& get_ith(size_t idx) const {
        return triangles[idx];
    }

    inline const std::vector<Triangle>::const_iterator begin() const {
        return triangles.begin();
    }

    inline const std::vector<Triangle>::const_iterator end() const {
        return triangles.end();
    }

    Box get_bounding_box(
        std::vector<int>::const_iterator begin,
        std::vector<int>::const_iterator end
    ) const;
private:
    std::vector<Triangle> triangles;
};
