#pragma once

#include <vector>
#include "utils/triangle.hpp"
#include <cstddef>

class TriangularMesh {
public:
    TriangularMesh(): triangles() {}

    TriangularMesh(std::vector<Triangle>& data): triangles(data) {}

    TriangularMesh(std::vector<Triangle>&& data): triangles(std::move(data)) {}

    inline size_t size() const {
        return triangles.size();
    }

    inline Triangle get_ith(size_t idx) const {
        return triangles.at(idx);
    }
private:
    std::vector<Triangle> triangles;
};
