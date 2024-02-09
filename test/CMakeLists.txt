project(test)

add_library(catch_static STATIC catch.cpp)

add_executable(tests
    sample_test1.cpp
    sample_test2.cpp
)

target_link_libraries(tests catch_static)

add_test(NAME tests COMMAND tests)
