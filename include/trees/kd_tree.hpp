#pragma once

#include <memory>
#include "meshes/triangular_mesh.hpp"


class KDTree {
public:
    KDTree(const TriangularMesh& mesh): mesh(mesh) {
    }
private:
    const TriangularMesh& mesh;
};
