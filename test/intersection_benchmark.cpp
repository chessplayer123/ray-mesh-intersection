#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include <fstream>
#include <iostream>
#include "trees/kd_tree.hpp"
#include "readers/reader.hpp"
#include "utils/ray.hpp"
#include "utils/triangle.hpp"
#include "rmi_algorithm.hpp"

TEST_CASE("Linear traverse", "[benchmark][ray][mesh]") {
    std::string filename = "../../data/bunny.ply";
    TriangularMesh mesh = read_triangular_mesh(filename);
    Ray ray(Vector(0, 0, 0), Vector(1, 0, 0));

    BENCHMARK("Linear search " + std::to_string(mesh.size())) {
        return ray.intersects(mesh);
    };
}

TEST_CASE("K-D Tree traverse", "[benchmark][ray][kdtree]") {
    std::string filename = "../../data/bunny.ply";
    TriangularMesh mesh = read_triangular_mesh(filename);
    auto kdtree = KDTree::for_mesh(mesh);
    Ray ray(Vector(0, 0, 0), Vector(1, 0, 0));

    BENCHMARK("K-D tree search " + std::to_string(mesh.size())) {
        return ray.intersects(kdtree);
    };
}
