#ifndef VECTOR_HPP_
#define VECTOR_HPP_

struct Vector {
    double x;
    double y;
    double z;

    Vector(): x(0), y(0), z(0) {}
    Vector(double x, double y, double z): x(x), y(y), z(z) {}

    double length() const;
    Vector operator+(const Vector& rhs) const;
    Vector operator-(const Vector& rhs) const;
    Vector operator*(double value) const;
    Vector cross_mul(const Vector& rhs);
    double dot_mul(const Vector& rhs);
    Vector ort() const;
};

typedef Vector Point;

#endif // VECTOR_HPP_
