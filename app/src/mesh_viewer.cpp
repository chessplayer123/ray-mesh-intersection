#include "mesh_viewer.hpp"
#include "rmilib/parallel_algos.hpp"

#include <QDebug>
#include <QPainter>
#include <QTimer>
#include <chrono>


using namespace std::chrono;


MeshViewer::MeshViewer(QWidget* parent):
    QOpenGLWidget(parent),
    click_point(std::nullopt),
    always_find_intersections(true),
    camera(rmi::Vector3d(10, 10, 10), width(), height())
{
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MeshViewer::updateFrame);
    timer->start(10);
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
    for (const auto& triangle : *mesh) {
        glBegin(GL_TRIANGLES);
            glVertex3d(triangle.v<0>().x(), triangle.v<0>().y(), triangle.v<0>().z());
            glVertex3d(triangle.v<1>().x(), triangle.v<1>().y(), triangle.v<1>().z());
            glVertex3d(triangle.v<2>().x(), triangle.v<2>().y(), triangle.v<2>().z());
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
    time_point<steady_clock> last_frame,
    size_t size,
    const rmi::Vector3d& pos,
    const rmi::Vector3d& dir,
    size_t intersections_count,
    const QString& intersection_time
) {
    long time_spent = duration_cast<milliseconds>(steady_clock::now() - last_frame).count();
    int fps = 1e3 / static_cast<int>(time_spent);

    return QString(
        "FPS: %0 (%1 ms)\n"
        "Number of polygons: %2\n"
        "Camera (%3, %4, %5)\n"
        "Direction (%6, %7, %8)\n\n"
        "Found %9 intersections (%10)"
    )
        .arg(fps).arg(time_spent)
        .arg(size)
        .arg(pos.x()).arg(pos.y()).arg(pos.z())
        .arg(dir.x()).arg(dir.y()).arg(dir.z())
        .arg(intersections_count).arg(intersection_time);
}

void MeshViewer::drawDescription() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    QPainter painter(this);
    painter.setPen(Qt::white);
    painter.setBrush(Qt::black);
    painter.setFont(QFont("Times New Roman", 16));
    QRect rect(0, 0, 600, 250);
    painter.drawRect(rect);
    painter.drawText(
        rect,
        Qt::AlignLeft | Qt::AlignTop,
        prepareDescription(
            last_frame,
            mesh->size(),
            camera.position(),
            camera.direction(),
            intersections.size(),
            intersection_time_spent
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
    if (!mesh) return;

    switch (event->key()) {
        case Qt::Key_Space:   always_find_intersections = !always_find_intersections; break;
        case Qt::Key_W:       camera.move_forward(.5);  break;
        case Qt::Key_S:       camera.move_backward(.5); break;
        case Qt::Key_A:       camera.move_left(.5);     break;
        case Qt::Key_D:       camera.move_right(.5);    break;
        case Qt::Key_Shift:   camera.move_up(.5);       break;
        case Qt::Key_Control: camera.move_down(.5);     break;
        default: return;
    }
}

void MeshViewer::updateFrame() {
    if (!mesh) return;
    last_frame = steady_clock::now();

    if (always_find_intersections) {
        findIntersections();
    }
    update();
}

void MeshViewer::findIntersections() {
    auto start = steady_clock::now();
    intersections = camera.eye_ray().intersects(*tree);
    auto sync_time_delta = duration_cast<microseconds>(steady_clock::now() - start).count();

    intersection_time_spent = QString("sync: %0 us").arg(sync_time_delta);

    #ifdef RMI_INCLUDE_OMP
        start = steady_clock::now();
        intersections = rmi::parallel_intersects_omp(camera.eye_ray(), *tree, 2);
        auto omp_time_delta = duration_cast<microseconds>(steady_clock::now() - start).count();
        intersection_time_spent += QString("\n    omp: %0 us (x %1)")
            .arg(omp_time_delta)
            .arg(static_cast<double>(sync_time_delta)/static_cast<double>(omp_time_delta));
    #endif

    #ifdef RMI_INCLUDE_POOL
        start = steady_clock::now();
        intersections = rmi::parallel_intersects_pool(camera.eye_ray(), *tree, 2);
        auto pool_time_delta = duration_cast<microseconds>(steady_clock::now() - start).count();
        intersection_time_spent += QString("\n    pool: %0 us (x %1)")
            .arg(pool_time_delta)
            .arg(static_cast<double>(sync_time_delta)/static_cast<double>(pool_time_delta));
    #endif
}

void MeshViewer::mousePressEvent(QMouseEvent* event) {
    click_point = event->pos();
}

void MeshViewer::mouseMoveEvent(QMouseEvent* event) {
    if (!mesh || !click_point.has_value()) return;

    camera.rotate(
        event->pos().x() - click_point.value().x(),
        -event->pos().y() + click_point.value().y()
    );

    click_point = event->pos();
}

void MeshViewer::setMesh(TriangularMesh&& new_mesh) {
    mesh = std::make_unique<TriangularMesh>(new_mesh);
    tree = std::make_unique<rmi::KDTree<TriangularMesh>>(
        rmi::KDTree<TriangularMesh>::for_mesh(mesh->begin(), mesh->end())
    );
    updateFrame();
}
