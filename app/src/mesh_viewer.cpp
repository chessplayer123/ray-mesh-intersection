#include "mesh_viewer.hpp"
#include <iostream>


MeshViewer::MeshViewer(QWidget* parent): QOpenGLWidget(parent) {
}

void MeshViewer::initializeGL() {
    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void MeshViewer::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void MeshViewer::paintGL() {
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

void MeshViewer::mousePressEvent(QMouseEvent* event) {
}

void MeshViewer::mouseMoveEvent(QMouseEvent* event) {
}

void MeshViewer::setMesh(TriangularMesh&& mesh) {
    this->mesh = mesh;
    update();
}
