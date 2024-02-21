#include <catch2/catch.hpp>
#include "utils/triangle.hpp"


TEST_CASE("Barycentric coordinates") {
}

TEST_CASE("Contains point method") {
    GIVEN("Triangle") {
        Vector p1(0.3, -15, 9.7), p2(2.3, 0.01, -1), p3(1, 2, 8);
        Triangle triangle(p1, p2, p3);
        WHEN("Checking whether triangle contains its corner points") {
            bool contains_all_corners = triangle.contains(p1)
                && triangle.contains(p2)
                && triangle.contains(p3);
            THEN("Should return true") {
                REQUIRE(contains_all_corners == true);
            }
        }

        WHEN("Checking whether triangle contains its internal point") {
            THEN("Should return true") {
            }
        }
    }
}
