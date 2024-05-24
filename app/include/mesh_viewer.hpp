#pragma once

#include <optional>
#include <QOpenGLWidget>
#include <QMouseEvent>
#include "rmilib/kd_tree.hpp"
#include "rmilib/raw_mesh.hpp"
#include "camera.hpp"

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
    void updateFrame();

    std::unique_ptr<TriangularMesh> mesh;
    std::unique_ptr<KDTree<TriangularMesh>> tree;

    std::optional<QPoint> click_point;
    std::vector<Vector3d> intersections;
    QString intersection_time_spent;
    bool always_find_intersections;

    std::chrono::time_point<std::chrono::steady_clock> last_frame;

    Camera camera;
};
