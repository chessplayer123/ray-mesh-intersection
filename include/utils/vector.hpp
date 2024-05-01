#pragma once

#include <iostream>


class Vector {
    friend std::ostream& operator<<(std::ostream& stream, const Vector& vec);
public:

    Vector(): m_x(0), m_y(0), m_z(0) {}
    Vector(double x, double y, double z): m_x(x), m_y(y), m_z(z) {}

    Vector operator+(const Vector& rhs) const;
    Vector operator-(const Vector& rhs) const;
    Vector operator*(double value) const;
    Vector operator*(const Vector& rhs) const;
    Vector operator/(double value) const;
    bool operator==(const Vector& rhs) const;
    bool operator!=(const Vector& rhs) const;
    bool equals(const Vector& rhs, double epsilon) const;

    double length() const;
    double angle(const Vector& rhs) const;
    Vector cross(const Vector& rhs) const;
    double dot(const Vector& rhs) const;
    Vector ort() const;
    inline double min() const {
        return std::min(m_x, std::min(m_y, m_z));
    }
    inline double max() const {
        return std::max(m_x, std::max(m_y, m_z));
    }

    inline double x() const { return m_x; }
    inline double y() const { return m_y; }
    inline double z() const { return m_z; }

private:
    double m_x;
    double m_y;
    double m_z;
};
