#include <catch2/catch.hpp>

#include <sstream>
#include "utils/ray.hpp"
#include "utils/triangle.hpp"
#include "readers/reader.hpp"

#define EPSILON 0.00001

TEST_CASE("Ray and triangle intersection method", "[ray][triangle]") {
    GIVEN("Ray parallel to triangle") {
        Ray ray(Vector(0, 0, 10), Vector(1, 0, 0));
        Triangle triangle(Vector(1, 0, 0), Vector(0, 1, 0), Vector(1, 1, 0));
        WHEN("Finding intersection") {
            std::optional<Vector> intersection = ray.intersects(triangle);
            THEN("Should return std::nullopt") {
                REQUIRE(intersection.has_value() == false);
            }
        }
    }

    GIVEN("Ray perpendicular to triangle without intersection") {
        Ray ray(Vector(10, 0, 0), Vector(1, 0, 0));
        Triangle triangle(Vector(1, 0, 0), Vector(0, 1, 0), Vector(1, 1, 0));
        WHEN("Finding intersection") {
            std::optional<Vector> actualIntersection = ray.intersects(triangle);
            THEN("Should return std::nullopt") {
                REQUIRE(actualIntersection.has_value() == false);
            }
        }
    }

    GIVEN("Ray perpendicular to triangle with intersection") {
        Ray ray(Vector(10, 1, 1), Vector(-1, 0, 0));
        Triangle triangle(Vector(0, 1, 0), Vector(0, 0, 1), Vector(0, 1, 1));
        WHEN("Finding intersection") {
            std::optional<Vector> actualIntersection = ray.intersects(triangle);
            THEN("Should return expected intersection point") {
                Vector expectedIntersection(0, 1, 1);
                REQUIRE(actualIntersection.has_value());
                REQUIRE(actualIntersection.value() == expectedIntersection);
            }
        }
    }

    GIVEN("Ray positioned randomly relative to triangle with intersection") {
        Ray ray(Vector(0.4, 0.2, 1.3), Vector(-1.3, 0.3, -7));
        Triangle triangle(Vector(-1, 0.5, 2.3), Vector(0, -0.15, 0), Vector(1.3, 1, 1));
        WHEN("Finding intersection") {
            std::optional<Vector> actualIntersection = ray.intersects(triangle);
            THEN("Should return expected intersection point") {
                Vector expectedIntersection(0.253933, 0.233708, 0.513483);
                REQUIRE(actualIntersection.has_value());
                REQUIRE(actualIntersection.value().equals(expectedIntersection, EPSILON));
            }
        }
    }

    GIVEN("Ray positioned randomly relative to triangle without intersection") {
        Ray ray(Vector(0.4, 0.2, 1.3), Vector(1.3, -0.3, 7));
        Triangle triangle(Vector(-1, 0.5, 2.3), Vector(0, -0.15, 0), Vector(1.3, 1, 1));
        WHEN("Finding intersection") {
            std::optional<Vector> actualIntersection = ray.intersects(triangle);
            THEN("Should return std::nullopt") {
                REQUIRE(actualIntersection.has_value() == false);
            }
        }
    }
}

TEST_CASE("Ray and bounding box intersection method", "[ray][aabb]") {
    GIVEN("Ray outside AABB with intersection") {
        Ray ray(Vector(2, 2, 2), Vector(-1, -1, -1));
        AABBox aabb(Vector(-1, -1, -1), Vector(1, 1, 1));
        WHEN("Checking whether they intersects") {
            bool is_intersects = ray.intersects(aabb);
            THEN("Should return true") {
                REQUIRE(is_intersects == true);
            }
        }
    }

    GIVEN("Ray outside AABB without intersection") {
        Ray ray(Vector(10, 2, 2), Vector(1, 1, 1));
        AABBox aabb(Vector(-1, -1, -1), Vector(1, 1, 1));
        WHEN("Checking whether they intersects") {
            bool is_intersects = ray.intersects(aabb);
            THEN("Should return false") {
                REQUIRE(is_intersects == false);
            }
        }
    }

    GIVEN("Ray above AABB without intersection") {
        Ray ray(Vector(2, 2, 2), Vector(-1, -1, 0));
        AABBox aabb(Vector(-1, -1, -1), Vector(1, 1, 1));
        WHEN("Checking whether they intersects") {
            bool is_intersects = ray.intersects(aabb);
            THEN("Should return false") {
                REQUIRE(is_intersects == false);
            }
        }
    }

    GIVEN("Ray inside AABB") {
        Ray ray(Vector(0, 0, 0), Vector(1, 0, 0));
        AABBox aabb(Vector(-1, -1, -1), Vector(1, 1, 1));
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
        std::vector<Triangle> triangles;
        std::vector<Vector> expected_intersections;
        for (double x = 1; x <= 1000; ++x) {
            triangles.emplace_back(
                Vector(x, 0, 0),
                Vector(x, 1, 0),
                Vector(x, 0, 1)
            );
            expected_intersections.emplace_back(x, 0, 0);
        }
        Ray ray(Vector(0, 0, 0), Vector(1, 0, 0));
        auto mesh = std::make_shared<TriangularMesh>(triangles);


        WHEN("Finding intersections with mesh") {
            auto actual_intersections = ray.intersects(*mesh);
            REQUIRE_THAT(
                actual_intersections,
                Catch::Matchers::UnorderedEquals(expected_intersections)
            );
        }

        WHEN("Finding intersections with kdtree") {
            auto kdtree = KDTree::for_mesh(mesh);
            auto actual_intersections = ray.intersects(kdtree);
            REQUIRE_THAT(
                actual_intersections,
                Catch::Matchers::UnorderedEquals(expected_intersections)
            );
        }
    }
}
