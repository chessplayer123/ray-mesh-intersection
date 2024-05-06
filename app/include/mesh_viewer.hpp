#pragma once

#include <optional>
#include <QOpenGLWidget>
#include <QMouseEvent>
#include "camera.hpp"
#include "rmilib/triangular_mesh.hpp"
#include "rmilib/kd_tree.hpp"

class MeshViewer: public QOpenGLWidget {
public:
    MeshViewer(QWidget* parent = nullptr);

    void setMesh(TriangularMesh&& mesh);
    void findIntersections();
protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
private:
    void drawMesh();
    void drawDescription();

    std::shared_ptr<TriangularMesh> mesh;
    std::unique_ptr<KDTree> tree;
    std::optional<QPoint> click_point = std::nullopt;
    std::vector<Vector> intersections;
    Camera camera;
};
