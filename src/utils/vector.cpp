#include "utils/vector.hpp"
#include "utils/misc.hpp"
#include <math.h>


std::ostream& operator<<(std::ostream& stream, const Vector& vec) {
    stream << "Vector(" << vec.m_x << ", " << vec.m_y << ", " << vec.m_z << ")";
    return stream;
}

double Vector::length() const {
    return sqrt(m_x*m_x + m_y*m_y + m_z*m_z);
}

double Vector::angle(const Vector& rhs) const {
    double my_len = length();
    double other_len = rhs.length();
    double dot_mul = dot(rhs);
    double cos_value = cast_range(-1.0, dot_mul / (my_len * other_len), 1.0);

    return acos(cos_value);
}

Vector Vector::operator+(const Vector& rhs) const {
    return Vector(m_x + rhs.m_x, m_y + rhs.m_y, m_z + rhs.m_z);
}

bool Vector::operator==(const Vector& rhs) const {
    return m_x == rhs.m_x && m_y == rhs.m_y && m_z == rhs.m_z;
}

bool Vector::operator!=(const Vector& rhs) const {
    return m_x != rhs.m_x || m_y != rhs.m_y || m_z != rhs.m_z;
}

bool Vector::equals(const Vector& rhs, double epsilon) const {
    return abs(m_x - rhs.m_x) < epsilon
        && abs(m_y - rhs.m_y) < epsilon
        && abs(m_z - rhs.m_z) < epsilon;
}

Vector Vector::operator-(const Vector& rhs) const {
    return Vector(m_x - rhs.m_x, m_y - rhs.m_y, m_z - rhs.m_z);
}

Vector Vector::operator*(double value) const {
    return Vector(m_x * value, m_y * value, m_z * value);
}

Vector Vector::operator*(const Vector& rhs) const {
    return Vector(m_x * rhs.m_x, m_y * rhs.m_y, m_z * rhs.m_z);
}

Vector Vector::operator/(double value) const {
    return Vector(m_x / value, m_y / value, m_z / value);
}

Vector Vector::cross(const Vector& rhs) const {
    return Vector(
        m_y * rhs.m_z - m_z * rhs.m_y,
        m_z * rhs.m_x - m_x * rhs.m_z,
        m_x * rhs.m_y - m_y * rhs.m_x
    );
}

double Vector::dot(const Vector& rhs) const {
    return m_x * rhs.m_x + m_y * rhs.m_y + m_z * rhs.m_z;
}

Vector Vector::ort() const {
    double len = length();
    return Vector(m_x / len, m_y / len, m_z / len);
}
