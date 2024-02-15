#ifndef VECTOR_HPP_
#define VECTOR_HPP_

#include <iostream>

struct Vector;


class Vector {
public:
    friend std::ostream& operator<<(std::ostream& stream, const Vector& vec);

    Vector(): x(0), y(0), z(0) {}
    Vector(double x, double y, double z): x(x), y(y), z(z) {}

    Vector operator+(const Vector& rhs) const;
    Vector operator-(const Vector& rhs) const;
    Vector operator*(double value) const;
    bool operator==(const Vector& rhs) const;

    double length() const;
    double angle(const Vector& rhs) const;
    Vector cross(const Vector& rhs) const;
    double dot(const Vector& rhs) const;
    Vector ort() const;

    inline double get_x() { return x; }
    inline double get_y() { return y; }
    inline double get_z() { return z; }

private:
    double x;
    double y;
    double z;

};

#endif // VECTOR_HPP_
