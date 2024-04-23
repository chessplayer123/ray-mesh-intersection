#include "utils/vector.hpp"
#include "utils/misc.hpp"
#include <math.h>


std::ostream& operator<<(std::ostream& stream, const Vector& vec) {
    stream << "Vector(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return stream;
}

double Vector::length() const {
    return sqrt(x*x + y*y + z*z);
}

double Vector::angle(const Vector& rhs) const {
    double my_len = length();
    double other_len = rhs.length();
    double dot_mul = dot(rhs);
    double cos_value = cast_range(-1.0, dot_mul / (my_len * other_len), 1.0);

    return acos(cos_value);
}

Vector Vector::operator+(const Vector& rhs) const {
    return Vector(x + rhs.x, y + rhs.y, z + rhs.z);
}

bool Vector::operator==(const Vector& rhs) const {
    return x == rhs.x && y == rhs.y && z == rhs.z;
}

bool Vector::equals(const Vector& rhs, double epsilon) const {
    return abs(x - rhs.x) < epsilon
        && abs(y - rhs.y) < epsilon
        && abs(z - rhs.z) < epsilon;
}

Vector Vector::operator-(const Vector& rhs) const {
    return Vector(x - rhs.x, y - rhs.y, z - rhs.z);
}

Vector Vector::operator*(double value) const {
    return Vector(x * value, y * value, z * value);
}

Vector Vector::cross(const Vector& rhs) const {
    return Vector(
        y * rhs.z - z * rhs.y,
        x * rhs.z - z * rhs.x,
        x * rhs.y - y * rhs.x
    );
}

double Vector::dot(const Vector& rhs) const {
    return x * rhs.x + y * rhs.y + z * rhs.z;
}

Vector Vector::ort() const {
    double len = length();
    return Vector(x / len, y / len, z / len);
}
