#include <QDebug>
#include <QPainter>
#include <chrono>
#include "mesh_viewer.hpp"
#include "utils/ray.hpp"


MeshViewer::MeshViewer(QWidget* parent):
    QOpenGLWidget(parent), camera(Vector(10, 10, 10), width(), height())
{
}

void MeshViewer::initializeGL() {
    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);
    glPointSize(10);
}

void MeshViewer::resizeGL(int w, int h) {
    camera.resize(w, h);
}

void MeshViewer::drawMesh() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor3f(1.0, 1.0, 1.0);
    for (const Triangle& triangle : *mesh) {
        glBegin(GL_TRIANGLES);
            glVertex3d(triangle.v1().x(), triangle.v1().y(), triangle.v1().z());
            glVertex3d(triangle.v2().x(), triangle.v2().y(), triangle.v2().z());
            glVertex3d(triangle.v3().x(), triangle.v3().y(), triangle.v3().z());
        glEnd();
    }

    if (!intersections.empty()) {
        glColor3f(1.0, 0.0, 0.0);
        glBegin(GL_POINTS);
        for (const auto& point : intersections) {
            glVertex3d(point.x(), point.y(), point.z());
        }
        glEnd();
    }
}

QString prepareDescription(
    size_t size,
    const Vector& pos,
    const Vector& dir,
    size_t intersections_count
) {
    return QString(
        "Number of polygons: %0\n"
        "Camera (%1, %2, %3)\n"
        "Direction (%4, %5, %6)\n\n"
        "Found %7 intersections"
    )
        .arg(size)
        .arg(pos.x()).arg(pos.y()).arg(pos.z())
        .arg(dir.x()).arg(dir.y()).arg(dir.z())
        .arg(intersections_count);
}

void MeshViewer::drawDescription() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    QPainter painter(this);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Times New Roman", 16));
    painter.drawText(
        QRect(0, 0, 500, 500),
        Qt::AlignLeft | Qt::AlignTop,
        prepareDescription(
            mesh->size(),
            camera.position(),
            camera.direction(),
            intersections.size()
        )
    );
    painter.end();
}

void MeshViewer::paintGL() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glClear(GL_COLOR_BUFFER_BIT);

    if (mesh) {
        camera.setup();
        drawMesh();
        drawDescription();
    }
}

void MeshViewer::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
        case Qt::Key_W:       camera.move_forward(.5);  update(); break;
        case Qt::Key_S:       camera.move_backward(.5); update(); break;
        case Qt::Key_A:       camera.move_left(.5);     update(); break;
        case Qt::Key_D:       camera.move_right(.5);    update(); break;
        case Qt::Key_Shift:   camera.move_up(.5);       update(); break;
        case Qt::Key_Control: camera.move_down(.5);     update(); break;
        case Qt::Key_Space:
            findIntersections();
            update();
            break;
    }
}

void MeshViewer::findIntersections() {
    using namespace std::chrono;

    auto start = steady_clock::now();
    intersections = camera.eye_ray().intersects(*tree);
    long delta = duration_cast<microseconds>(steady_clock::now() - start).count();
    qDebug() << "[INFO] Found:" << intersections.size() << "intersections."
             << "Spent:" << delta << "us";

    update();
}

void MeshViewer::mousePressEvent(QMouseEvent* event) {
    click_point = event->pos();
}

void MeshViewer::mouseMoveEvent(QMouseEvent* event) {
    if (!click_point.has_value()) return;

    camera.rotate(
        event->pos().x() - click_point.value().x(),
        -event->pos().y() + click_point.value().y()
    );

    click_point = event->pos();
    update();
}

void MeshViewer::setMesh(TriangularMesh&& mesh) {
    this->mesh = std::make_shared<TriangularMesh>(mesh);
    tree = std::make_unique<KDTree>(KDTree::for_mesh(this->mesh));
    update();
}
