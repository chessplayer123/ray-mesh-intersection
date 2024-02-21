#ifndef MISC_HPP_
#define MISC_HPP_

inline double cast_range(double left, double value, double right) {
    if (value < left) return left;
    else if (right < value) return right;
    return value;
}

inline bool inside_inclusive_range(double left, double value, double right) {
    return left <= value && value <= right;
}

inline bool inside_exclusive_range(double left, double value, double right) {
    return left < value && value < right;
}

#endif // MISC_HPP_
