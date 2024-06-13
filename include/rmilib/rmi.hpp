#pragma once


#include <iostream>
#include <optional>
#include <limits>
#include <vector>
#include <cstddef>
#include <numeric>
#include <memory>
#include <algorithm>
#include <math.h>


namespace rmi {

template<typename T>
class Vector3 {
public:
    template<typename E>
    friend std::ostream& operator<<(std::ostream& stream, const Vector3<E>& vec);

    constexpr Vector3();
    constexpr Vector3(T x, T y, T z);

    void    operator+=(const Vector3& rhs);
    void    operator-=(const Vector3& rhs);
    Vector3 operator+ (const Vector3& rhs) const;
    Vector3 operator- (const Vector3& rhs) const;
    Vector3 operator* (T value) const;
    Vector3 operator* (const Vector3& rhs) const;
    Vector3 operator/ (T value) const;
    bool    operator==(const Vector3& rhs) const;
    bool    operator!=(const Vector3& rhs) const;

    bool    equals(const Vector3& rhs, double epsilon) const;
    T       length() const;
    T       dot(const Vector3& rhs) const;
    Vector3 cross(const Vector3& rhs) const;
    Vector3 ort() const;
    T       x() const;
    T       y() const;
    T       z() const;
private:
    T m_x;
    T m_y;
    T m_z;
};

typedef Vector3<double> Vector3d;

typedef Vector3<float> Vector3f;


// Axis-aligned bounding box
template<typename T>
struct AABBox {
    AABBox();
    AABBox(Vector3<T> min, Vector3<T> max);

    void operator+=(const AABBox<T>& box);

    Vector3<T> min;
    Vector3<T> max;
};


template<typename T>
class Range {
public:
    Range(T begin, T end): m_begin(begin), m_end(end) {}

    inline T begin() const { return m_begin; }
    inline T end()   const { return m_end; }
private:
    T m_begin;
    T m_end;
};


template<typename T, int N>
class Tree {
public:
    typedef typename T::iterator mesh_iterator;

    class Node {
    public:
        friend Tree;

        Node(const Node&)            = delete;
        Node()                       = delete;
        Node& operator=(const Node&) = delete;

        Node(Node&&)            = default;
        Node& operator=(Node&&) = default;
        ~Node()                 = default;

        Node(
            AABBox<typename T::float_t> box,
            Range<mesh_iterator> range,
            std::vector<Node>&& children
        ): bounding_box(box), range(range), children(std::move(children)) {}

        static Node build(typename T::iterator begin, typename T::iterator end, int depth_limit);

        inline bool                               is_leaf()     const { return children.empty(); }
        inline const std::vector<Node>&           child_nodes() const { return children; }
        inline const AABBox<typename T::float_t>& box()         const { return bounding_box; }
        inline const Range<mesh_iterator>&        triangles()   const { return range; }
    private:
        AABBox<typename T::float_t> bounding_box;
        Range<mesh_iterator> range;
        std::vector<Node> children;
    };

    inline static Tree<T, N> for_mesh(mesh_iterator begin, mesh_iterator end, int depth_limit = 16)
    { return Tree<T, N>(std::move(Node::build(begin, end, depth_limit))); }

    inline const Node& top() const
    { return root; }
private:
    Tree(Node&& root): root(std::move(root)) {}

    Node root;
};


template<typename T>
using KDTree = Tree<T, 1>;

template<typename T>
using Quadtree = Tree<T, 2>;

template<typename T>
using Octree = Tree<T, 3>;


template<typename mesh_type, typename float_type, typename index_type>
class Mesh {
public:
    struct Element {
        template<index_type vertex_num>
        inline constexpr Vector3<float_type> v() const
        { return mesh->template v<vertex_num>(index); }

        index_type index;
        Vector3<float_type> center;
        const mesh_type* mesh;
    };

    using float_t = float_type;
    using index_t = index_type;
    using iterator = typename std::vector<Element>::iterator;

    Mesh(index_t size);

    iterator begin();
    iterator end();
private:
    void setup();

    bool is_setup;
    std::vector<Element> elements;
};


template<typename float_t>
class Ray {
public:
    Ray(Vector3<float_t> origin, Vector3<float_t> direction);

    bool intersects(const AABBox<float_t>& box) const;

    template<typename mesh_t>
    std::optional<Vector3<float_t>> intersects(
        const typename mesh_t::Element& triangle,
        float_t epsilon = std::numeric_limits<float_t>::epsilon()
    ) const;

    template<typename mesh_t, typename index_t>
    std::vector<Vector3<float_t>> intersects(
        Mesh<mesh_t, float_t, index_t>& mesh,
        float_t epsilon = std::numeric_limits<float_t>::epsilon()
    ) const;

    template<typename T, int N>
    std::vector<Vector3<float_t>> intersects(
        const Tree<T, N>& tree,
        float_t epsilon = std::numeric_limits<float_t>::epsilon()
    ) const;
private:
    template<typename T, int N>
    void recursive_intersects(
        const typename Tree<T, N>::Node& iter,
        std::vector<Vector3<float_t>>& output,
        float_t epsilon = std::numeric_limits<float_t>::epsilon()
    ) const;

    Vector3<float_t> origin;
    Vector3<float_t> vector;
    Vector3<float_t> inv_vector;
};

} // namespace rmi


namespace rmi {

// Vector3 implementation
template<typename T>
inline constexpr Vector3<T>::Vector3(): m_x(0), m_y(0), m_z(0) {
}

template<typename T>
inline constexpr Vector3<T>::Vector3(T x, T y, T z): m_x(x), m_y(y), m_z(z) {
}

template<typename T>
inline std::ostream& operator<<(std::ostream& stream, const Vector3<T>& vec) {
    return stream << "Vector(" << vec.m_x << ", " << vec.m_y << ", " << vec.m_z << ")";
}

template<typename T>
inline Vector3<T> Vector3<T>::operator+(const Vector3<T>& rhs) const {
    return Vector3<T>(
        m_x + rhs.m_x,
        m_y + rhs.m_y,
        m_z + rhs.m_z
    );
}

template<typename T>
inline void Vector3<T>::operator+=(const Vector3<T>& rhs) {
    m_x += rhs.m_x;
    m_y += rhs.m_y;
    m_z += rhs.m_z;
}

template<typename T>
inline Vector3<T> Vector3<T>::operator-(const Vector3<T>& rhs) const {
    return Vector3<T>(m_x - rhs.m_x, m_y - rhs.m_y, m_z - rhs.m_z);
}

template<typename T>
inline void Vector3<T>::operator-=(const Vector3<T>& rhs) {
    m_x -= rhs.m_x;
    m_y -= rhs.m_y;
    m_z -= rhs.m_z;
}

template<typename T>
inline Vector3<T> Vector3<T>::operator*(T value) const {
    return Vector3<T>(m_x * value, m_y * value, m_z * value);
}

template<typename T>
inline Vector3<T> Vector3<T>::operator*(const Vector3<T>& rhs) const {
    return Vector3<T>(m_x * rhs.m_x, m_y * rhs.m_y, m_z * rhs.m_z);
}

template<typename T>
inline Vector3<T> Vector3<T>::operator/(T value) const {
    return Vector3<T>(m_x / value, m_y / value, m_z / value);
}

template<typename T>
inline bool Vector3<T>::operator==(const Vector3<T>& rhs) const {
    return m_x == rhs.m_x && m_y == rhs.m_y && m_z == rhs.m_z;
}

template<typename T>
inline bool Vector3<T>::operator!=(const Vector3<T>& rhs) const {
    return m_x != rhs.m_x || m_y != rhs.m_y || m_z != rhs.m_z;
}

template<typename T>
inline bool Vector3<T>::equals(const Vector3<T>& rhs, double epsilon) const {
    return abs(m_x - rhs.m_x) < epsilon
        && abs(m_y - rhs.m_y) < epsilon
        && abs(m_z - rhs.m_z) < epsilon;
}

template<typename T>
inline T Vector3<T>::length() const {
    return sqrt(m_x*m_x + m_y*m_y + m_z*m_z);
}

template<typename T>
inline Vector3<T> Vector3<T>::cross(const Vector3<T>& rhs) const {
    return Vector3<T>(
        m_y * rhs.m_z - m_z * rhs.m_y,
        m_z * rhs.m_x - m_x * rhs.m_z,
        m_x * rhs.m_y - m_y * rhs.m_x
    );
}

template<typename T>
inline T Vector3<T>::dot(const Vector3<T>& rhs) const {
    return m_x * rhs.m_x + m_y * rhs.m_y + m_z * rhs.m_z;
}

template<typename T>
inline Vector3<T> Vector3<T>::ort() const {
    T len = length();
    return Vector3<T>(m_x / len, m_y / len, m_z / len);
}

template<typename T>
inline T Vector3<T>::x() const {
    return m_x;
}

template<typename T>
inline T Vector3<T>::y() const {
    return m_y;
}

template<typename T>
inline T Vector3<T>::z() const {
    return m_z;
}


// AABBox implementation
template<typename T>
AABBox<typename T::float_t> get_bounding_box(
    typename T::iterator begin,
    typename T::iterator end
) {
    using std::min, std::max;

    typename T::float_t min_x, min_y, min_z, max_x, max_y, max_z;
    min_x = min_y = min_z = std::numeric_limits<typename T::float_t>::max();
    max_x = max_y = max_z = std::numeric_limits<typename T::float_t>::min();

    for (auto cur = begin; cur != end; ++cur) {
        const Vector3 v1 = cur->template v<0>();
        const Vector3 v2 = cur->template v<1>();
        const Vector3 v3 = cur->template v<2>();

        min_x = min(min_x, min(v1.x(), min(v2.x(), v3.x())));
        max_x = max(max_x, max(v1.x(), max(v2.x(), v3.x())));

        max_y = max(max_y, max(v1.y(), max(v2.y(), v3.y())));
        min_y = min(min_y, min(v1.y(), min(v2.y(), v3.y())));

        max_z = max(max_z, max(v1.z(), max(v2.z(), v3.z())));
        min_z = min(min_z, min(v1.z(), min(v2.z(), v3.z())));
    }

    return {
        Vector3(min_x, min_y, min_z),
        Vector3(max_x, max_y, max_z)
    };
}

template<typename T>
AABBox<T>::AABBox() {
    T mn = std::numeric_limits<T>::min();
    T mx = std::numeric_limits<T>::max();

    min = Vector3<T>(mx, mx, mx);
    max = Vector3<T>(mn, mn, mn);
}

template<typename T>
AABBox<T>::AABBox(Vector3<T> min, Vector3<T> max): min(min), max(max) {
}

template<typename T>
void AABBox<T>::operator+=(const AABBox<T>& box) {
    min = Vector3(
        std::min(min.x(), box.min.x()),
        std::min(min.y(), box.min.y()),
        std::min(min.z(), box.min.z())
    );
    max = Vector3(
        std::max(max.x(), box.max.x()),
        std::max(max.y(), box.max.y()),
        std::max(max.z(), box.max.z())
    );
}


// Mesh implementation
template<typename mesh_t, typename float_t, typename index_t>
Mesh<mesh_t, float_t, index_t>::Mesh(index_t size): is_setup(false), elements(size) {
}

template<typename mesh_t, typename float_t, typename index_t>
inline typename Mesh<mesh_t, float_t, index_t>::iterator Mesh<mesh_t, float_t, index_t>::begin() {
    if (!is_setup) setup();
    return elements.begin();
}

template<typename mesh_t, typename float_t, typename index_t>
inline typename Mesh<mesh_t, float_t, index_t>::iterator Mesh<mesh_t, float_t, index_t>::end() {
    if (!is_setup) setup();
    return elements.end();
}

template<typename mesh_t, typename float_t, typename index_t>
void Mesh<mesh_t, float_t, index_t>::setup() {
    is_setup = true;
    auto child = static_cast<const mesh_t*>(this);
    for (index_t i = 0; i < elements.size(); ++i) {
        elements[i] = {
            i,
            (child->template v<0>(i) + child->template v<1>(i) + child->template v<2>(i)) / 3,
            child
        };
    }
}


// Tree implementation
template<typename It>
void split(
    It begin, It end,
    std::vector<Range<It>>& ranges,
    int depth_limit,
    size_t counter
) {
    size_t length = std::distance(begin, end);
    if (length <= 0) {
        return;
    } else if (length <= static_cast<size_t>(1 << counter) || counter == 0) {
        ranges.emplace_back(begin, end);
        return;
    }

    switch ((depth_limit + counter) % 3) {
    case 0:
        std::sort(begin, end, [](const auto& it1, const auto& it2) {
            return it1.center.x() < it2.center.x();
        }); break;
    case 1:
        std::sort(begin, end, [](const auto& it1, const auto& it2) {
            return it1.center.y() < it2.center.y();
        }); break;
    case 2:
        std::sort(begin, end, [](const auto& it1, const auto& it2) {
            return it1.center.z() < it2.center.z();
        }); break;
    }

    auto mid = std::next(begin, length / 2);
    split(begin, mid, ranges, depth_limit, counter - 1);
    split(mid, end, ranges, depth_limit, counter - 1);
}


template<typename T, int N>
typename Tree<T, N>::Node Tree<T, N>::Node::build(
    typename T::iterator begin,
    typename T::iterator end,
    int depth_limit
) {
    if (depth_limit <= 0) {
        return Node(
            get_bounding_box<T>(begin, end),
            Range(begin, end),
            std::vector<typename Tree<T, N>::Node>()
        );
    }

    std::vector<Range<typename T::iterator>> ranges;
    split(begin, end, ranges, depth_limit, N);

    std::vector<typename Tree<T, N>::Node> children;
    AABBox<typename T::float_t> aabb;
    for (const auto& range : ranges) {
        auto child = build(range.begin(), range.end(), depth_limit - 1);
        aabb += child.box();
        children.push_back(std::move(child));
    }

    return Node(aabb, Range(begin, end), std::move(children));
}


// Ray implementation
/*
 * Read for details:
 * https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
 */

template<typename float_t>
Ray<float_t>::Ray(Vector3<float_t> origin, Vector3<float_t> direction):
    origin(origin),
    vector(direction.ort())
{
    inv_vector = Vector3<float_t>(
        1.0 / vector.x(),
        1.0 / vector.y(),
        1.0 / vector.z()
    );
}

template<typename float_t>
template<typename mesh_t>
std::optional<Vector3<float_t>> Ray<float_t>::intersects(
    const typename mesh_t::Element& triangle,
    float_t epsilon
) const {
    const Vector3 v1 = triangle.template v<0>();
    const Vector3 v2 = triangle.template v<1>();
    const Vector3 v3 = triangle.template v<2>();

    Vector3 edge1 = v2 - v1;
    Vector3 edge2 = v3 - v1;
    Vector3 ray_cross_e2 = vector.cross(edge2);

    float_t det = edge1.dot(ray_cross_e2);
    if (-epsilon <= det && det <= epsilon) {
        return std::nullopt;
    }
    float_t inv_det = 1.0 / det;

    Vector3 s = origin - v1;
    float_t u = inv_det * s.dot(ray_cross_e2);
    if (u < 0.0 || u > 1.0) {
        return std::nullopt;
    }

    Vector3 s_cross_e1 = s.cross(edge1);
    float_t v = inv_det * vector.dot(s_cross_e1);
    if (v < 0.0 || u + v > 1.0) {
        return std::nullopt;
    }

    float_t t = inv_det * edge2.dot(s_cross_e1);
    if (t <= epsilon) {
        return std::nullopt;
    }
    return origin + vector * t;
}


template<typename float_t>
template<typename T, int N>
void Ray<float_t>::recursive_intersects(
    const typename Tree<T, N>::Node& node,
    std::vector<Vector3<float_t>>& output,
    float_t epsilon
) const {
    if (node.is_leaf()) {
        for (const auto& cur : node.triangles()) {
            auto intersection = intersects<T>(cur, epsilon);
            if (intersection.has_value()) {
                output.push_back(intersection.value());
            }
        }
    } else {
        for (const auto& child : node.child_nodes()) {
            if (intersects(child.box())) {
                recursive_intersects<T, N>(child, output, epsilon);
            }
        }
    }
}


template<typename float_t>
template<typename T, int N>
std::vector<Vector3<float_t>> Ray<float_t>::intersects(
    const Tree<T, N>& tree,
    float_t epsilon
) const {
    const typename Tree<T, N>::Node& node = tree.top();
    if (!intersects(node.box())) {
        return {};
    }

    std::vector<Vector3<float_t>> output;
    recursive_intersects<T, N>(node, output, epsilon);
    return output;
}


/*
 * Simple iterative intersection search
 */
template<typename float_t>
template<typename mesh_t, typename index_t>
std::vector<Vector3<float_t>> Ray<float_t>::intersects(
    Mesh<mesh_t, float_t, index_t>& mesh,
    float_t epsilon
) const {
    std::vector<Vector3<float_t>> intersections;

    for (const auto& cur : mesh) {
        auto intersection = intersects<mesh_t>(cur, epsilon);

        if (intersection.has_value()) {
            intersections.push_back(intersection.value());
        }
    }

    return intersections;
}

template<typename float_t>
inline bool Ray<float_t>::intersects(const AABBox<float_t>& box) const {
    Vector3 t1 = (box.min - origin) * inv_vector;
    Vector3 t2 = (box.max - origin) * inv_vector;

    float_t tmin = std::min(t1.x(), t2.x());
    float_t tmax = std::max(t1.x(), t2.x());

    tmin = std::max(tmin, std::min(t1.y(), t2.y()));
    tmax = std::min(tmax, std::max(t1.y(), t2.y()));

    tmin = std::max(tmin, std::min(t1.z(), t2.z()));
    tmax = std::min(tmax, std::max(t1.z(), t2.z()));

    return tmax >= 0 && tmin <= tmax;
}

} // namespace rmi
