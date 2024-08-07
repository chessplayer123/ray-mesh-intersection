project(test)

add_library(CATCH_STATIC STATIC catch.cpp)
target_compile_definitions(CATCH_STATIC PRIVATE 
    -DCATCH_CONFIG_ENABLE_BENCHMARKING
)

file(GLOB TEST_SOURCES *_test.cpp *_benchmark.cpp)

set(LIBS CATCH_STATIC rmilib)
set(DEFS "")

if (INCLUDE_OMP)
    if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -openmp:llvm")
    else()
        find_package(OpenMP REQUIRED)
        list(APPEND LIBS OpenMP::OpenMP_CXX)
    endif()
    list(APPEND DEFS RMI_INCLUDE_OMP)
endif()

if (INCLUDE_POOL)
    list(APPEND DEFS RMI_INCLUDE_POOL)
endif()

foreach(TEST_FILE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    target_link_libraries(${TEST_NAME} PRIVATE ${LIBS})
    target_compile_definitions(${TEST_NAME} PRIVATE ${DEFS})

    if (${TEST_NAME} MATCHES "benchmark$")
        set_tests_properties(${TEST_NAME} PROPERTIES LABELS "benchmark")
    else()
        set_tests_properties(${TEST_NAME} PROPERTIES LABELS "unittest")
    endif()
endforeach(TEST_FILE ${TEST_SOURCES})
