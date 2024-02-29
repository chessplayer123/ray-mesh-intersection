#include <catch2/catch.hpp>
#include "utils/ray.hpp"
#include "utils/triangle.hpp"

#define EPSILON 0.00001

TEST_CASE("Ray intersection method", "[ray]") {
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
                REQUIRE(actualIntersection.value().equals(expectedIntersection, EPSILON));
            }
        }
    }

    GIVEN("Ray positioned randomly relative to triangle with intersection") {
        Ray ray(Vector(0.4, 0.2, 1.3), Vector(1.3, -0.3, 7));
        Triangle triangle(Vector(-1, 0.5, 2.3), Vector(0, -0.15, 0), Vector(1.3, 1, 1));
        WHEN("Finding intersection") {
            std::optional<Vector> actualIntersection = ray.intersects(triangle);
            THEN("Should return expected intersection point") {
                Vector expectedIntersection(0.454454, 0.187434, 1.59321);
                REQUIRE(actualIntersection.has_value());
                REQUIRE(actualIntersection.value().equals(expectedIntersection, EPSILON));
            }
        }
    }

    GIVEN("Ray positioned randomly relative to triangle without intersection") {
        Ray ray(Vector(0.4, 0.2, 1.3), Vector(-1.3, 0.3, -7));
        Triangle triangle(Vector(-1, 0.5, 2.3), Vector(0, -0.15, 0), Vector(1.3, 1, 1));
        WHEN("Finding intersection") {
            std::optional<Vector> actualIntersection = ray.intersects(triangle);
            THEN("Should return std::nullopt") {
                REQUIRE(actualIntersection.has_value() == false);
            }
        }
    }
}
