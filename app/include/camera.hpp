#pragma once

#include "rmilib/vector.hpp"
#include "rmilib/ray.hpp"


class Camera {
public:
    Camera(Vector pos, int width, int height):
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

    inline const Vector& direction() const { return front; }
    inline const Vector& position() const { return pos; }

    void resize(int w, int h);
    void rotate(double yaw_degree, double pitch_degree);

    void setup() const;
    Ray eye_ray() const;
private:
    Vector pos;

    Vector left;
    Vector up;
    Vector front;

    double yaw;
    double pitch;

    int width;
    int height;

    double fov;
    double view_distance;
};

