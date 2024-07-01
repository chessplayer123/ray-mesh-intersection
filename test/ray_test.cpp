#include <catch2/catch.hpp>

#include <sstream>
#include <iostream>
#include <array>
#include "rmilib/rmi.hpp"
#include "rmilib/raw_mesh.hpp"

#define EPSILON 0.00001


// Mesh consisting of a single triangle
struct MeshMock: rmi::Mesh<MeshMock> {
    using float_t = double;
    using index_t = size_t;

    template<size_t vertex_num>
    inline const rmi::Vector3d& v(size_t) const {
        return vertexes[vertex_num];
    }

    MeshMock(rmi::Vector3d v1, rmi::Vector3d v2, rmi::Vector3d v3):
        vertexes{v1, v2, v3}
    {
        rmi::Mesh<MeshMock>::setup(1);
    }

    std::array<rmi::Vector3d, 3> vertexes;
};


TEST_CASE("Ray and triangle intersection method", "[ray][triangle]") {
    GIVEN("Ray parallel to triangle") {
        rmi::Ray<double> ray(rmi::Vector3d(0, 0, 10), rmi::Vector3d(1, 0, 0));
        MeshMock mesh(rmi::Vector3d(1, 0, 0), rmi::Vector3d(0, 1, 0), rmi::Vector3d(1, 1, 0));
        auto triangle = *mesh.begin();
        WHEN("Finding intersection") {
            auto intersection = ray.intersects<MeshMock>(triangle);
            THEN("Should return std::nullopt") {
                REQUIRE(intersection.has_value() == false);
            }
        }
    }

    GIVEN("Ray perpendicular to triangle without intersection") {
        rmi::Ray<double> ray(rmi::Vector3d(10, 0, 0), rmi::Vector3d(1, 0, 0));
        MeshMock mesh(rmi::Vector3d(1, 0, 0), rmi::Vector3d(0, 1, 0), rmi::Vector3d(1, 1, 0));

        auto triangle = *mesh.begin();
        WHEN("Finding intersection") {
            auto actualIntersection = ray.intersects<MeshMock>(triangle);
            THEN("Should return std::nullopt") {
                REQUIRE(actualIntersection.has_value() == false);
            }
        }
    }

    GIVEN("Ray perpendicular to triangle with intersection") {
        rmi::Ray<double> ray(rmi::Vector3d(10, 1, 1), rmi::Vector3d(-1, 0, 0));
        MeshMock mesh(rmi::Vector3d(0, 1, 0), rmi::Vector3d(0, 0, 1), rmi::Vector3d(0, 1, 1));
        auto triangle = *mesh.begin();
        WHEN("Finding intersection") {
            auto actualIntersection = ray.intersects<MeshMock>(triangle);
            THEN("Should return expected intersection point") {
                REQUIRE(actualIntersection.has_value());
            }
        }
    }

    GIVEN("Ray positioned randomly relative to triangle with intersection") {
        rmi::Ray<double> ray(rmi::Vector3d(0.4, 0.2, 1.3), rmi::Vector3d(-1.3, 0.3, -7));
        MeshMock mesh(rmi::Vector3d(-1, 0.5, 2.3), rmi::Vector3d(0, -0.15, 0), rmi::Vector3d(1.3, 1, 1));
        auto triangle = *mesh.begin();
        WHEN("Finding intersection") {
            auto actualIntersection = ray.intersects<MeshMock>(triangle);
            THEN("Should return expected intersection point") {
                REQUIRE(actualIntersection.has_value());
            }
        }
    }

    GIVEN("Ray positioned randomly relative to triangle without intersection") {
        rmi::Ray<double> ray(rmi::Vector3d(0.4, 0.2, 1.3), rmi::Vector3d(1.3, -0.3, 7));
        MeshMock mesh(rmi::Vector3d(-1, 0.5, 2.3), rmi::Vector3d(0, -0.15, 0), rmi::Vector3d(1.3, 1, 1));
        auto triangle = *mesh.begin();
        WHEN("Finding intersection") {
            auto actualIntersection = ray.intersects<MeshMock>(triangle);
            THEN("Should return std::nullopt") {
                REQUIRE(actualIntersection.has_value() == false);
            }
        }
    }
}

TEST_CASE("Ray and bounding box intersection method", "[ray][aabb]") {
    GIVEN("Ray outside AABB with intersection") {
        rmi::Ray<double> ray(rmi::Vector3d(2, 2, 2), rmi::Vector3d(-1, -1, -1));
        rmi::AABBox<double> aabb = {rmi::Vector3d(-1, -1, -1), rmi::Vector3d(1, 1, 1)};
        WHEN("Checking whether they intersects") {
            bool is_intersects = ray.is_intersects(aabb);
            THEN("Should return true") {
                REQUIRE(is_intersects == true);
            }
        }
    }

    GIVEN("Ray outside AABB without intersection") {
        rmi::Ray<double> ray(rmi::Vector3d(10, 2, 2), rmi::Vector3d(1, 1, 1));
        rmi::AABBox<double> aabb = {rmi::Vector3d(-1, -1, -1), rmi::Vector3d(1, 1, 1)};
        WHEN("Checking whether they intersects") {
            bool is_intersects = ray.is_intersects(aabb);
            THEN("Should return false") {
                REQUIRE(is_intersects == false);
            }
        }
    }

    GIVEN("Ray above AABB without intersection") {
        rmi::Ray<double> ray(rmi::Vector3d(2, 2, 2), rmi::Vector3d(-1, -1, 0));
        rmi::AABBox<double> aabb = {rmi::Vector3d(-1, -1, -1), rmi::Vector3d(1, 1, 1)};
        WHEN("Checking whether they intersects") {
            bool is_intersects = ray.is_intersects(aabb);
            THEN("Should return false") {
                REQUIRE(is_intersects == false);
            }
        }
    }

    GIVEN("Ray inside AABB") {
        rmi::Ray<double> ray(rmi::Vector3d(0, 0, 0), rmi::Vector3d(1, 0, 0));
        rmi::AABBox<double> aabb = {rmi::Vector3d(-1, -1, -1), rmi::Vector3d(1, 1, 1)};
        WHEN("Checking whether they intersects") {
            bool is_intersects = ray.is_intersects(aabb);
            THEN("Should return true") {
                REQUIRE(is_intersects == true);
            }
        }
    }
}

TEST_CASE("Ray and triangular mesh intersection methods", "[ray][mesh][kdtree]") {
    GIVEN("Triangular mesh and ray") {
        std::vector<double> coords;
        std::vector<size_t> indices;
        std::vector<rmi::Vector3<double>> expected_intersections;
        for (double x = 1.; x <= 1000.; ++x) {
            coords.push_back(x); coords.push_back(0); coords.push_back(0);
            coords.push_back(x); coords.push_back(1); coords.push_back(0);
            coords.push_back(x); coords.push_back(0); coords.push_back(1);

            indices.push_back((x-1.)*3. + 0.);
            indices.push_back((x-1.)*3. + 1.);
            indices.push_back((x-1.)*3. + 2.);

            expected_intersections.emplace_back(x, 0, 0);
        }

        rmi::Ray<double> ray(rmi::Vector3d(0, 0, 0), rmi::Vector3d(1, 0, 0));
        TriangularMesh mesh(std::move(coords), std::move(indices));

        WHEN("Finding intersections with mesh") {
            auto actual_intersections = ray.intersects(mesh);
            REQUIRE_THAT(
                actual_intersections,
                Catch::Matchers::UnorderedEquals(expected_intersections)
            );
        }

        WHEN("Finding intersections with kdtree built with SAH") {
            auto kdtree = rmi::KDTree<TriangularMesh>::for_mesh(mesh, rmi::SAHSplitter<TriangularMesh>());
            auto actual_intersections = ray.intersects(kdtree);
            REQUIRE_THAT(
                actual_intersections,
                Catch::Matchers::UnorderedEquals(expected_intersections)
            );
        }

        WHEN("Finding intersections with kdtree built with mid splitter") {
            auto kdtree = rmi::KDTree<TriangularMesh>::for_mesh(mesh, rmi::MedianSplitter<TriangularMesh>());
            auto actual_intersections = ray.intersects(kdtree);
            REQUIRE_THAT(
                actual_intersections,
                Catch::Matchers::UnorderedEquals(expected_intersections)
            );
        }

        #ifdef RMI_INCLUDE_POOL
        WHEN("Finding intersections with kdtree with pool parallel algorithm") {
            auto kdtree = rmi::KDTree<TriangularMesh>::for_mesh(mesh);
            auto actual_intersections = ray.pool_intersects(kdtree, 2);
            REQUIRE_THAT(
                actual_intersections,
                Catch::Matchers::UnorderedEquals(expected_intersections)
            );
        }
        #endif

        #ifdef RMI_INCLUDE_OMP
        WHEN("Finding intersections with kdtree with omp parallel algorithm") {
            auto kdtree = rmi::KDTree<TriangularMesh>::for_mesh(mesh);
            auto actual_intersections = ray.omp_intersects(kdtree, 2);
            REQUIRE_THAT(
                actual_intersections,
                Catch::Matchers::UnorderedEquals(expected_intersections)
            );
        }

        WHEN("Finding intersections with mesh in parallel") {
            auto actual_intersections = ray.omp_intersects(mesh, 2);
            REQUIRE_THAT(
                actual_intersections,
                Catch::Matchers::UnorderedEquals(expected_intersections)
            );
        }
        #endif
    }
}
