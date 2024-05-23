#include <catch2/catch.hpp>

#include <sstream>
#include <array>
#include "rmilib/ray.hpp"
#include "rmilib/reader.hpp"

#define EPSILON 0.00001

// Mesh consisting of a single triangle
struct MeshMock: Mesh<MeshMock> {
    template<size_t vertex_num>
    inline const Vector& v(size_t) const {
        return vertexes[vertex_num];
    }

    MeshMock(Vector v1, Vector v2, Vector v3): Mesh<MeshMock>(1), vertexes{v1, v2, v3} {
    }

    std::array<Vector, 3> vertexes;
};


TEST_CASE("Ray and triangle intersection method", "[ray][triangle]") {
    GIVEN("Ray parallel to triangle") {
        Ray ray(Vector(0, 0, 10), Vector(1, 0, 0));
        MeshMock mesh(Vector(1, 0, 0), Vector(0, 1, 0), Vector(1, 1, 0));
        auto triangle = *mesh.begin();
        WHEN("Finding intersection") {
            std::optional<Vector> intersection = ray.intersects<MeshMock>(triangle);
            THEN("Should return std::nullopt") {
                REQUIRE(intersection.has_value() == false);
            }
        }
    }

    GIVEN("Ray perpendicular to triangle without intersection") {
        Ray ray(Vector(10, 0, 0), Vector(1, 0, 0));
        MeshMock mesh(Vector(1, 0, 0), Vector(0, 1, 0), Vector(1, 1, 0));
        auto triangle = *mesh.begin();
        WHEN("Finding intersection") {
            std::optional<Vector> actualIntersection = ray.intersects<MeshMock>(triangle);
            THEN("Should return std::nullopt") {
                REQUIRE(actualIntersection.has_value() == false);
            }
        }
    }

    GIVEN("Ray perpendicular to triangle with intersection") {
        Ray ray(Vector(10, 1, 1), Vector(-1, 0, 0));
        MeshMock mesh(Vector(0, 1, 0), Vector(0, 0, 1), Vector(0, 1, 1));
        auto triangle = *mesh.begin();
        WHEN("Finding intersection") {
            std::optional<Vector> actualIntersection = ray.intersects<MeshMock>(triangle);
            THEN("Should return expected intersection point") {
                Vector expectedIntersection(0, 1, 1);
                REQUIRE(actualIntersection.has_value());
                REQUIRE(actualIntersection.value() == expectedIntersection);
            }
        }
    }

    GIVEN("Ray positioned randomly relative to triangle with intersection") {
        Ray ray(Vector(0.4, 0.2, 1.3), Vector(-1.3, 0.3, -7));
        MeshMock mesh(Vector(-1, 0.5, 2.3), Vector(0, -0.15, 0), Vector(1.3, 1, 1));
        auto triangle = *mesh.begin();
        WHEN("Finding intersection") {
            std::optional<Vector> actualIntersection = ray.intersects<MeshMock>(triangle);
            THEN("Should return expected intersection point") {
                Vector expectedIntersection(0.253933, 0.233708, 0.513483);
                REQUIRE(actualIntersection.has_value());
                REQUIRE(actualIntersection.value().equals(expectedIntersection, EPSILON));
            }
        }
    }

    GIVEN("Ray positioned randomly relative to triangle without intersection") {
        Ray ray(Vector(0.4, 0.2, 1.3), Vector(1.3, -0.3, 7));
        MeshMock mesh(Vector(-1, 0.5, 2.3), Vector(0, -0.15, 0), Vector(1.3, 1, 1));
        auto triangle = *mesh.begin();
        WHEN("Finding intersection") {
            std::optional<Vector> actualIntersection = ray.intersects<MeshMock>(triangle);
            THEN("Should return std::nullopt") {
                REQUIRE(actualIntersection.has_value() == false);
            }
        }
    }
}

TEST_CASE("Ray and bounding box intersection method", "[ray][aabb]") {
    GIVEN("Ray outside AABB with intersection") {
        Ray ray(Vector(2, 2, 2), Vector(-1, -1, -1));
        AABBox aabb = {Vector(-1, -1, -1), Vector(1, 1, 1)};
        WHEN("Checking whether they intersects") {
            bool is_intersects = ray.intersects(aabb);
            THEN("Should return true") {
                REQUIRE(is_intersects == true);
            }
        }
    }

    GIVEN("Ray outside AABB without intersection") {
        Ray ray(Vector(10, 2, 2), Vector(1, 1, 1));
        AABBox aabb = {Vector(-1, -1, -1), Vector(1, 1, 1)};
        WHEN("Checking whether they intersects") {
            bool is_intersects = ray.intersects(aabb);
            THEN("Should return false") {
                REQUIRE(is_intersects == false);
            }
        }
    }

    GIVEN("Ray above AABB without intersection") {
        Ray ray(Vector(2, 2, 2), Vector(-1, -1, 0));
        AABBox aabb = {Vector(-1, -1, -1), Vector(1, 1, 1)};
        WHEN("Checking whether they intersects") {
            bool is_intersects = ray.intersects(aabb);
            THEN("Should return false") {
                REQUIRE(is_intersects == false);
            }
        }
    }

    GIVEN("Ray inside AABB") {
        Ray ray(Vector(0, 0, 0), Vector(1, 0, 0));
        AABBox aabb = {Vector(-1, -1, -1), Vector(1, 1, 1)};
        WHEN("Checking whether they intersects") {
            bool is_intersects = ray.intersects(aabb);
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
        std::vector<Vector> expected_intersections;
        for (double x = 1; x <= 1000; ++x) {
            coords.push_back(x); coords.push_back(0); coords.push_back(0);
            coords.push_back(x); coords.push_back(1); coords.push_back(0);
            coords.push_back(x); coords.push_back(0); coords.push_back(1);

            indices.push_back((x-1)*3 + 0);
            indices.push_back((x-1)*3 + 1);
            indices.push_back((x-1)*3 + 2);

            expected_intersections.emplace_back(x, 0, 0);
        }
        Ray ray(Vector(0, 0, 0), Vector(1, 0, 0));
        TriangularMesh mesh(std::move(coords), std::move(indices));

        WHEN("Finding intersections with mesh") {
            auto actual_intersections = ray.intersects(mesh);
            REQUIRE_THAT(
                actual_intersections,
                Catch::Matchers::UnorderedEquals(expected_intersections)
            );
        }

        WHEN("Finding intersections with kdtree") {
            auto kdtree = KDTree<TriangularMesh>::for_mesh(mesh.begin(), mesh.end());
            auto actual_intersections = ray.intersects(kdtree);
            REQUIRE_THAT(
                actual_intersections,
                Catch::Matchers::UnorderedEquals(expected_intersections)
            );
        }
    }
}
