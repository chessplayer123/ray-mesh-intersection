#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include <iostream>
#include <random>
#include <sstream>
#include <fstream>
#include <time.h>

#include "rmilib/raw_mesh.hpp"
#include "rmilib/reader.hpp"
#include "rmilib/rmi.hpp"
#include "rmilib/rmi_parallel.hpp" 


const std::string filename = "../../data/Fantasy_Castle.stl";
TriangularMesh mesh = read_raw_triangular_mesh<double, size_t>(filename);

using Splitter = rmi::SAHSplitter<TriangularMesh>;


class SampleGenerator {
public:
    SampleGenerator(int seed): seed(seed), engine(seed), dist(-1, 1) {}

    rmi::Ray<double> next_ray() {
        return rmi::Ray<double>(
            rmi::Vector3d(dist(engine), dist(engine), dist(engine)),
            rmi::Vector3d(dist(engine), dist(engine), dist(engine))
        );
    }

    void reset() {
        engine.seed(seed);
    }

    int seed;
    std::default_random_engine engine;
    std::uniform_real_distribution<> dist;
} generator(23487);


template<typename... Args>
std::string concat(const Args&... args) {
    std::stringstream stream;
    (stream << ... << args);
    return stream.str();
}


TEST_CASE("Mesh intersection", "[benchmark][ray][mesh]") {
    generator.reset();
    BENCHMARK_ADVANCED(concat("Sequential search ", mesh.size()))(auto meter) {
        auto ray = generator.next_ray();
        meter.measure([&ray] { return ray.intersects(mesh); });
    };

#ifdef RMI_INCLUDE_OMP
    for (int threads_count = 2; threads_count <= 8; threads_count *= 2) {
        generator.reset();
        BENCHMARK_ADVANCED(concat("Parallel <", threads_count, "> search ", mesh.size()))(auto meter) {
            auto ray = generator.next_ray();
            meter.measure([&ray, threads_count] {
                return rmi::parallel::omp_intersects(ray, mesh, threads_count);
            });
        };
    }
#endif
}


TEST_CASE("KD-Tree intersection", "[benchmark][ray][kdtree]") {
    auto tree = rmi::KDTree<TriangularMesh>::for_mesh(mesh.begin(), mesh.end(), Splitter());

    generator.reset();
    BENCHMARK_ADVANCED(concat("Sync KD-Tree search "))(auto meter) {
        auto ray = generator.next_ray();
        meter.measure([&ray, &tree] { return ray.intersects(tree); });
    };

#ifdef RMI_INCLUDE_OMP
    for (int threads_count = 2; threads_count <= 8; threads_count *= 2) {
        generator.reset();
        BENCHMARK_ADVANCED(concat(
            "OMP (", threads_count, " threads) KD-Tree search "
        ))(auto meter) {
            auto ray = generator.next_ray();
            meter.measure([&ray, &tree, threads_count] {
                return rmi::parallel::omp_intersects(ray, tree, threads_count);
            });
        };
    }
#endif

#ifdef RMI_INCLUDE_POOL 
    for (int threads_count = 2; threads_count <= 8; threads_count *= 2) {
        generator.reset();
        BENCHMARK_ADVANCED(concat(
            "Thread pool (", threads_count, " threads) KD-Tree search "
        ))(auto meter) {
            auto ray = generator.next_ray();
            meter.measure([&ray, &tree, threads_count] {
                return rmi::parallel::pool_intersects(ray, tree, threads_count);
            });
        };
    }
#endif
}
