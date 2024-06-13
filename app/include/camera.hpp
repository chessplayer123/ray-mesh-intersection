#pragma once

#include "rmilib/rmi.h"

class Camera {
public:
    Camera(rmi::Vector3d pos, int width, int height):
        pos(pos),
        left(0, 0, -1), up(0, 1, 0), front(1, 0, 0),
        yaw(0.0), pitch(0.0),
        width(width), height(height),
        fov(45.0), view_distance(1000.0) {}

    inline void move_forward(double units) { pos += front * units; }
    inline void move_backward(double units) { pos -= front * units; }

    inline void move_left(double units) { pos += left * units; }
    inline void move_right(double units) { pos -= left * units; }

    inline void move_up(double units) { pos += up * units; }
    inline void move_down(double units) { pos -= up * units; }

    inline const rmi::Vector3d& direction() const { return front; }
    inline const rmi::Vector3d& position() const { return pos; }

    void resize(int w, int h);
    void rotate(double yaw_degree, double pitch_degree);

    void setup() const;

    rmi::Ray<double> eye_ray() const {
        return rmi::Ray(pos, front);
    }
private:
    rmi::Vector3d pos;

    rmi::Vector3d left;
    rmi::Vector3d up;
    rmi::Vector3d front;

    double yaw;
    double pitch;

    int width;
    int height;

    double fov;
    double view_distance;
};

