project(main-app)


# Main executable
add_executable(App main.cpp)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
target_link_libraries(App GL GLU glut)
target_link_libraries(App Library)
