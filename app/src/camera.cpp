#include "camera.hpp"
#include <GL/glu.h>
#include <math.h>


void Camera::rotate(double yaw_degree, double pitch_degree) {
    double yaw_rad = yaw_degree * M_PI / 180.0;
    double pitch_rad = pitch_degree * M_PI / 180.0;

    pitch = std::max(-M_PI_2, std::min(pitch + pitch_rad, M_PI_2));
    yaw += yaw_rad;

    front = rmi::Vector3d(
        cos(yaw) * cos(pitch),
        sin(pitch),
        sin(yaw) * cos(pitch)
    ).ort();
    left = rmi::Vector3d(cos(yaw - M_PI_2), 0, sin(yaw - M_PI_2)).ort();
    up = front.cross(left).ort();
}


void Camera::resize(int w, int h) {
    width = w;
    height = h;
    glViewport(0, 0, w, h);
}


void Camera::setup() const {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(
        fov,
        static_cast<double>(width) / static_cast<double>(height),
        .1, view_distance
    );
    rmi::Vector3d center = pos + front;
    gluLookAt(
        pos.x(), pos.y(), pos.z(),
        center.x(), center.y(), center.z(),
        .0, 1.0, .0
    );
}
