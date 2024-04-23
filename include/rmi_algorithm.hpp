#ifndef RMI_ALGORITHM_HPP_
#define RMI_ALOGRITHM_HPP_

#include <type_traits>
#include <vector>
#include <algorithm>
#include <numeric>
#include <optional>
#include "utils/vector.hpp"


#define ACCESSOR_FOR(T) \
    template<> \
    struct rmi::Accessor<T>

#define WITH_METHOD static inline auto


namespace rmi {

template<typename T>
struct Accessor {};


inline bool inside_inclusive_range(double left, double value, double right) {
    return left <= value && value <= right;
}

inline bool inside_exclusive_range(double left, double value, double right) {
    return left < value && value < right;
}

template <typename...>
using void_t = void;

template <typename, template <typename> class, typename = void_t<>>
struct exists : std::false_type {};

template <typename T, template <typename> class Op>
struct exists<T, Op, void_t<Op<T>>> : std::true_type {};

template<typename mesh_t>
class Intersector {
    using triangle_t = decltype(Accessor<mesh_t>::ith(std::declval<mesh_t>(), std::declval<size_t>()));
    using point_t = decltype(Accessor<triangle_t>::v1(std::declval<triangle_t>()));
    using v2_t = decltype(Accessor<triangle_t>::v2(std::declval<triangle_t>()));
    using v3_t = decltype(Accessor<triangle_t>::v3(std::declval<triangle_t>()));
    using coord_t = decltype(Accessor<point_t>::x(std::declval<point_t>()));

    static_assert(std::is_same_v<point_t, v2_t> && std::is_same_v<point_t, v3_t>, "Points types are not consinstent");
    static_assert(std::is_arithmetic_v<coord_t>, "Point values is not arithmetic");

    struct {
        inline Vector difference(const point_t& p1, const point_t& p2) {
            return Vector(
                Accessor<point_t>::x(p1) - Accessor<point_t>::x(p2),
                Accessor<point_t>::y(p1) - Accessor<point_t>::y(p2),
                Accessor<point_t>::z(p1) - Accessor<point_t>::z(p2)
            );
        }
    } point;
private:
    std::optional<Vector> intersects(
        const Vector& origin,
        const Vector& vector,
        const triangle_t& triangle,
        double epsilon
    ) {
        Vector edge1 = point.difference(
            Accessor<triangle_t>::v2(triangle),
            Accessor<triangle_t>::v1(triangle)
        );
        Vector edge2 = point.difference(
            Accessor<triangle_t>::v3(triangle),
            Accessor<triangle_t>::v1(triangle)
        );
        Vector ray_cross_e2 = vector.cross(edge2);

        double det = edge1.dot(ray_cross_e2);
        if (inside_exclusive_range(-epsilon, det, epsilon)) {
            return std::nullopt;
        }
        double inv_det = 1.0 / det;

        Vector s = origin - Accessor<triangle_t>::v1(triangle);
        double u = -inv_det * s.dot(ray_cross_e2);
        if (!inside_inclusive_range(0.0, u, 1.0)) {
            return std::nullopt;
        }

        Vector s_cross_e1 = edge1.cross(s);
        double v = inv_det * vector.dot(s_cross_e1);
        if (v < 0.0 || u + v > 1.0) {
            return std::nullopt;
        }

        double t = inv_det * edge2.dot(s_cross_e1);
        if (t <= epsilon) {
            return std::nullopt;
        }
        return Accessor<point_t>::create(
            origin.get_x() + vector.get_x() * t,
            origin.get_y() + vector.get_y() * t,
            origin.get_z() + vector.get_z() * t
        );
    }
public:
    Intersector(const mesh_t& mesh): mesh(mesh) {
    }

    template<typename ray_t>
    std::vector<point_t> with_ray(const ray_t& ray) {
        size_t mesh_size = Accessor<mesh_t>::size(mesh);
        auto ray_origin = Accessor<ray_t>::origin(ray);
        auto ray_vector = Accessor<ray_t>::vector(ray);
        auto origin = Vector(
            Accessor<point_t>::x(ray_origin),
            Accessor<point_t>::y(ray_origin),
            Accessor<point_t>::z(ray_origin)
        );
        auto vector = Vector(
            Accessor<point_t>::x(ray_vector),
            Accessor<point_t>::y(ray_vector),
            Accessor<point_t>::z(ray_vector)
        );

        std::vector<point_t> intersections;

        for (const triangle_t& triangle : mesh) {
            auto intersection = intersects(origin, vector, triangle, std::numeric_limits<double>::epsilon());
            if (intersection.has_value()) {
                intersections.push_back(intersection.value());
            }
        }

        return intersections;
    }
private:
    const mesh_t& mesh;
};

} // rmi namespace

#endif // RMI_ALOGRITHM_HPP_
