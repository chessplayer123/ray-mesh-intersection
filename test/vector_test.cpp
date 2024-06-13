#include <catch2/catch.hpp>
#include "rmilib/rmi.hpp"


TEST_CASE("Length method", "[vector]") {
    GIVEN("Vector with zero length") {
        rmi::Vector3d vec(0, 0, 0);
        double vec_length = vec.length();
        REQUIRE(vec_length == 0.0);
    }

    GIVEN("rmi::Vector3d with non-zero length") {
        rmi::Vector3d vec(1.0, -2.0, 2.0);
        double vec_length = vec.length();
        REQUIRE(vec_length == 3.0);
    }
}

TEST_CASE("Ort method", "[vector]") {
    GIVEN("Vector") {
        rmi::Vector3d vec(12.0, -0.15, -9.1);
        WHEN("Calling ort method") {
            rmi::Vector3d ort = vec.ort();
            THEN("Should return collinear rmi::Vector3d with length 1") {
                REQUIRE(ort.length() == 1.0);
            }
        }
    }
}


TEST_CASE("Binary operations", "[vector]") {
    GIVEN("Two vectors") {
        rmi::Vector3d vec1(1.0, -0.5, 2.0);
        rmi::Vector3d vec2(0.0, 0.5, 11.1);

        WHEN("Summing two vectors") {
            rmi::Vector3d actual_sum = vec1 + vec2;
            rmi::Vector3d expected_sum(1.0, 0.0, 13.1);

            THEN("Actual sum equals to expected") {
                REQUIRE(actual_sum == expected_sum );
            }
        }

        WHEN("Subtracting one rmi::Vector3d from another") {
            rmi::Vector3d actual_diff = vec1 - vec2;
            rmi::Vector3d expected_diff(1.0, -1.0, -9.1);

            THEN("Actual diff equals to expected") {
                REQUIRE(actual_diff == expected_diff);
            }
        }

        WHEN("Calculating dot product") {
            double actual_value = vec1.dot(vec2);
            double expected_value = 21.95;

            THEN("Actual value equals to expected") {
                REQUIRE(actual_value == expected_value);
            }
        }

        WHEN("Calculating cross product") {
            rmi::Vector3d actual_result = vec1.cross(vec2);
            rmi::Vector3d expected_result(-6.55, -11.1, 0.5);

            THEN("Actual result equals to expected") {
                REQUIRE(actual_result == expected_result);
            }
        }
    }
    GIVEN("One rmi::Vector3d and double value") {
        rmi::Vector3d vec1(1.0, -0.5, 2.0);
        double value = 2.0;
        WHEN("Multiplying rmi::Vector3d by some double value") {
            rmi::Vector3d actual_vector = vec1 * value;
            rmi::Vector3d expected_vector(2.0, -1.0, 4.0);

            THEN("Vector's coordinates should change as expected") {
                REQUIRE(actual_vector == expected_vector);
            }
        }
    }
}
