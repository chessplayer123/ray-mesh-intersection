project(qt-app)

# OpenGL
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

# QT
find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)

# Main executable
include_directories(include/)
file(GLOB SRCS src/*.cpp)
add_executable(qt_app src/main.cpp ${SRCS})
target_link_libraries(qt_app PRIVATE
    rmilib
    Qt5::Widgets
    Qt5::OpenGL
    ${OPENGL_LIBRARIES}
)

if (INCLUDE_OMP)
    target_link_libraries(qt_app PRIVATE OpenMP::OpenMP_CXX)
    target_compile_definitions(qt_app PRIVATE RMI_INCLUDE_OMP)
endif()

if (INCLUDE_POOL)
    target_link_libraries(qt_app PRIVATE atomic)
    target_compile_definitions(qt_app PRIVATE RMI_INCLUDE_POOL)
endif()
