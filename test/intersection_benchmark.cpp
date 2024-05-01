#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include <iostream>
#include <fstream>
#include <time.h>
#include "trees/kd_tree.hpp"
#include "readers/reader.hpp"
#include "utils/ray.hpp"
#include "utils/triangle.hpp"

double randd() {
    double v1 = static_cast<double>(rand());
    double v2 = static_cast<double>(rand());
    return std::min(v1, v2) / (std::max(v1, v2) + std::numeric_limits<double>::min());
}

TEST_CASE("Mesh intersection", "[benchmark][ray][mesh]") {
    srand(time(NULL));

    std::string filename = "../../data/bunny.ply";
    auto mesh = std::make_shared<const TriangularMesh>(read_triangular_mesh(filename));

    BENCHMARK_ADVANCED("Linear search " + std::to_string(mesh->size()))(auto meter) {
        Ray ray(
            Vector(0.0, 0.0, 0.0),
            Vector(randd(), randd(), randd())
        );

        meter.measure([&ray, &mesh] { return ray.intersects(*mesh); });
    };

    auto kdtree = KDTree::for_mesh(mesh);
    BENCHMARK_ADVANCED("K-D Tree search " + std::to_string(mesh->size()))(auto meter) {
        Ray ray(
            Vector(0.0, 0.0, 0.0),
            Vector(randd(), randd(), randd())
        );

        meter.measure([&ray, &kdtree] { return ray.intersects(kdtree); });
    };
}
