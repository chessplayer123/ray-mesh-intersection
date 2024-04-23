#pragma once

#include <QGLWidget>
#include <QOpenGLWidget>
#include <QMouseEvent>
#include "meshes/triangular_mesh.hpp"

class MeshViewer: public QOpenGLWidget {
public:
    MeshViewer(QWidget* parent = nullptr);

    void setMesh(TriangularMesh&& mesh);
protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
private:
    TriangularMesh mesh;
};
