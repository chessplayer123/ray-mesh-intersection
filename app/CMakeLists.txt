project(main-app)

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
