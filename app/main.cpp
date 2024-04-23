#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "meshes/triangular_mesh.hpp"
#include "mesh_readers/reader.hpp"
#include <fstream>
#include <cstring>

TriangularMesh mesh;

void setup() {
    std::ifstream stream;

    std::string filename = "../data/Utah_teapot_(solid).stl";
    stream.open(filename);
    mesh = MeshReader::read_triangular_mesh<MeshReader::Stl>(stream);
    std::cout << "[INFO] Triangular mesh from '" << filename << "' was successfully read (sz = " << mesh.size() << ")\n";

    stream.close();
}

void render(){
    glClear(GL_COLOR_BUFFER_BIT);   

    glColor3f(1.0, 1.0, 1.0);
    for (const Triangle& triangle : mesh) {
        glBegin(GL_POLYGON);
            glVertex3f(triangle.v1().get_x(), triangle.v1().get_y(), triangle.v1().get_z());
            glVertex3f(triangle.v2().get_x(), triangle.v2().get_y(), triangle.v2().get_z());
            glVertex3f(triangle.v3().get_x(), triangle.v3().get_y(), triangle.v3().get_z());
        glEnd();
    }

    glFlush();
}

int main(int argc, char ** argv) {
    glutInit(&argc, argv);  
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Model");

    glScalef(0.1, 0.1, 0.1);

    setup();
    glutDisplayFunc(&render);

    glutMainLoop();

    return 0;
}
