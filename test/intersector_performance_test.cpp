#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include <fstream>
#include <iostream>
#include "trees/kd_tree.hpp"
#include "mesh_readers/reader.hpp"
#include "utils/ray.hpp"
#include "utils/triangle.hpp"
#include "rmi_algorithm.hpp"

ACCESSOR_FOR(TriangularMesh) {
    WITH_METHOD ith(const TriangularMesh& mesh, size_t idx) {
        return mesh.get_ith(idx);
    }

    WITH_METHOD size(const TriangularMesh& mesh) {
        return mesh.size();
    }
};

ACCESSOR_FOR(Triangle) {
    WITH_METHOD v1(const Triangle& triangle) {
        return triangle.v1();
    }

    WITH_METHOD v2(const Triangle& triangle) {
        return triangle.v2();
    }

    WITH_METHOD v3(const Triangle& triangle) {
        return triangle.v3();
    }
};

ACCESSOR_FOR(Vector) {
    WITH_METHOD x(const Vector& vec) {
        return vec.get_x();
    }

    WITH_METHOD y(const Vector& vec) {
        return vec.get_y();
    }

    WITH_METHOD z(const Vector& vec) {
        return vec.get_z();
    }

    WITH_METHOD create(double x, double y, double z) {
        return Vector(x, y, z);
    }
};

ACCESSOR_FOR(Ray) {
    WITH_METHOD origin(const Ray& ray) {
        return ray.get_origin();
    }

    WITH_METHOD vector(const Ray& ray) {
        return ray.get_vector();
    }
};


TEST_CASE("Intersector") {
    std::ifstream stream;

    std::string filename = "../../data/bunny.ply";
    stream.open(filename);
    TriangularMesh mesh = MeshReader::read_triangular_mesh<MeshReader::Ply>(stream);
    Ray ray(Vector(0, 0, 0), Vector(1, 0, 0));

    stream.close();

    auto intersector = rmi::Intersector<TriangularMesh>(mesh);

    BENCHMARK("benchmark native " + std::to_string(mesh.size())) {
        return ray.intersects(mesh);
    };

    BENCHMARK("benchmark accessor " + std::to_string(mesh.size())) {
        return intersector.with_ray(ray);
    };

    BENCHMARK("benchmark convert") {
        std::vector<Triangle> triangles;
        for (Triangle triangle : mesh) {
            triangles.push_back(triangle);
        }
        TriangularMesh copy_mesh(triangles);
        return ray.intersects(std::move(copy_mesh));
    };
}
