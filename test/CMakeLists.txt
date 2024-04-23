project(test)

add_library(CATCH_STATIC STATIC catch.cpp)
target_compile_definitions(CATCH_STATIC PRIVATE 
    -DCATCH_CONFIG_ENABLE_BENCHMARKING
)

file(GLOB TEST_SOURCES *_test.cpp)

foreach(TEST_FILE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    target_link_libraries(${TEST_NAME} CATCH_STATIC Library)
endforeach(TEST_FILE ${TEST_SOURCES})
