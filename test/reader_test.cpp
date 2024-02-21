#include <catch2/catch.hpp>
#include "readers/reader.hpp"

TEST_CASE("Ply reader function test") {
    auto mesh = read_triangular_mesh<Ply>("");
}
