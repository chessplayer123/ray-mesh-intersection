#pragma once

#include <string>
#include "utils/result.hpp"
#include "meshes/triangular_mesh.hpp"


enum MeshFileFormat {
    Ply,
    Step,
    Auto, // define by name 
};

template<MeshFileFormat>
Result<TriangularMesh, std::string> read_triangular_mesh(const std::string& path);
