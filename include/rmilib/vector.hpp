#pragma once


#include <iostream>
#include <math.h>


template<typename T>
class Vector3 {
public:
    friend std::ostream& operator<<(std::ostream& stream, const Vector3& vec) {
        return stream << "Vector(" << vec.m_x << ", " << vec.m_y << ", " << vec.m_z << ")";
    }

    constexpr Vector3(): m_x(0), m_y(0), m_z(0) {}

    constexpr Vector3(T x, T y, T z): m_x(x), m_y(y), m_z(z) {}

    Vector3 operator+(const Vector3& rhs) const {
        return Vector3(m_x + rhs.m_x, m_y + rhs.m_y, m_z + rhs.m_z);
    }

    void operator+=(const Vector3& rhs) {
        m_x += rhs.m_x;
        m_y += rhs.m_y;
        m_z += rhs.m_z;
    }

    Vector3 operator-(const Vector3& rhs) const {
        return Vector3(m_x - rhs.m_x, m_y - rhs.m_y, m_z - rhs.m_z);
    }

    void operator-=(const Vector3& rhs) {
        m_x -= rhs.m_x;
        m_y -= rhs.m_y;
        m_z -= rhs.m_z;
    }

    Vector3 operator*(T value) const {
        return Vector3(m_x * value, m_y * value, m_z * value);
    }

    Vector3 operator*(const Vector3& rhs) const {
        return Vector3(m_x * rhs.m_x, m_y * rhs.m_y, m_z * rhs.m_z);
    }

    Vector3 operator/(T value) const {
        return Vector3(m_x / value, m_y / value, m_z / value);
    }

    bool operator==(const Vector3& rhs) const {
        return m_x == rhs.m_x && m_y == rhs.m_y && m_z == rhs.m_z;
    }

    bool operator!=(const Vector3& rhs) const {
        return m_x != rhs.m_x || m_y != rhs.m_y || m_z != rhs.m_z;
    }

    bool equals(const Vector3& rhs, double epsilon) const {
        return abs(m_x - rhs.m_x) < epsilon
            && abs(m_y - rhs.m_y) < epsilon
            && abs(m_z - rhs.m_z) < epsilon;
    }

    T length() const {
        return sqrt(m_x*m_x + m_y*m_y + m_z*m_z);
    }

    Vector3 cross(const Vector3& rhs) const {
        return Vector3(
            m_y * rhs.m_z - m_z * rhs.m_y,
            m_z * rhs.m_x - m_x * rhs.m_z,
            m_x * rhs.m_y - m_y * rhs.m_x
        );
    }

    T dot(const Vector3& rhs) const {
        return m_x * rhs.m_x + m_y * rhs.m_y + m_z * rhs.m_z;
    }

    Vector3 ort() const {
        T len = length();
        return Vector3(m_x / len, m_y / len, m_z / len);
    }

    inline T x() const { return m_x; }
    inline T y() const { return m_y; }
    inline T z() const { return m_z; }
private:
    T m_x;
    T m_y;
    T m_z;
};


typedef Vector3<double> Vector3d;

typedef Vector3<float> Vector3f;
