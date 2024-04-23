project(main-app)

# OpenGL
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

# QT
find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)

# Main executable
include_directories(include/)
file(GLOB SRCS src/*.cpp)
add_executable(App src/main.cpp ${SRCS})
target_link_libraries(App PRIVATE
    Library
    Qt5::Widgets
    Qt5::OpenGL
    ${OPENGL_LIBRARIES}
)
