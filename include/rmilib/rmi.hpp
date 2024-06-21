#pragma once


#include <optional>
#include <limits>
#include <vector>
#include <cstddef>
#include <numeric>
#include <memory>
#include <array>
#include <algorithm>
#include <math.h>


namespace rmi {

template<typename T>
class Vector3 {
public:
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
    T       operator[](int index) const;

    T           length() const;
    T           dot(const Vector3& rhs) const;
    Vector3     cross(const Vector3& rhs) const;
    Vector3     ort() const;
    constexpr T x() const;
    constexpr T y() const;
    constexpr T z() const;
private:
    std::array<T, 3> coords;
};

typedef Vector3<double> Vector3d;

typedef Vector3<float> Vector3f;


// Axis-aligned bounding box
template<typename T>
struct AABBox {
    AABBox();
    AABBox(Vector3<T> min, Vector3<T> max);

    void   operator+=(const AABBox<T>& box);
    AABBox operator+(const AABBox<T>& box) const;
    T      volume() const;

    Vector3<T> min;
    Vector3<T> max;
};


template<typename T>
class Range {
public:
    Range(T begin, T end): m_begin(begin), m_end(end) {}

    inline T    begin()  const { return m_begin; }
    inline T    end()    const { return m_end; }
    inline auto length() const { return std::distance(m_begin, m_end); }
private:
    T m_begin;
    T m_end;
};


template<typename T>
struct SAHSplitter {
    SAHSplitter(int threshold = 16): threshold(threshold) {}

    typename T::iterator operator()(
        const Range<typename T::iterator>& range,
        int depth
    ) const;

    std::pair<typename T::iterator, double> find_min_sah(
        const Range<typename T::iterator>& range
    ) const;

    int threshold;
};


template<typename T>
struct MedianSplitter {
    MedianSplitter(int depth_limit = 16): depth_limit(depth_limit) {}

    typename T::iterator operator()(
        const Range<typename T::iterator>& range,
        int depth
    ) const;

    int depth_limit;
};


template<typename T>
class KDTree {
public:
    typedef typename T::iterator mesh_iterator;

    class Node {
    public:
        Node(
            AABBox<typename T::float_t> box,
            Range<mesh_iterator> range,
            std::unique_ptr<Node>&& left,
            std::unique_ptr<Node>&& right
        ): bounding_box(box), range(range), m_left(std::move(left)), m_right(std::move(right)) {}

        Node(
            AABBox<typename T::float_t> box,
            Range<mesh_iterator> range
        ): bounding_box(box), range(range), m_left(nullptr), m_right(nullptr) {}

        template<typename Splitter>
        static std::unique_ptr<Node> build(const Range<typename T::iterator>& range, int depth, const Splitter& splitter);

        inline bool                               is_leaf()   const { return !m_left && !m_right; }
        inline bool                               has_left()  const { return static_cast<bool>(m_left); }
        inline bool                               has_right() const { return static_cast<bool>(m_right); }
        inline const Node&                        left()      const { return *m_left; }
        inline const Node&                        right()     const { return *m_right; }
        inline const AABBox<typename T::float_t>& box()       const { return bounding_box; }
        inline const Range<mesh_iterator>&        triangles() const { return range; }
    private:
        AABBox<typename T::float_t> bounding_box;
        Range<mesh_iterator> range;
        std::unique_ptr<Node> m_left;
        std::unique_ptr<Node> m_right;
    };

    KDTree(std::unique_ptr<Node>&& root): root(std::move(root)) {}

    template<typename Splitter = SAHSplitter<T>>
    inline static KDTree<T> for_mesh(
        mesh_iterator begin,
        mesh_iterator end,
        const Splitter& splitter = Splitter()
    ) {
        return KDTree<T>(std::move(Node::build(Range(begin, end), 0, splitter)));
    }

    inline const Node& top() const
    { return *root; }
private:
    std::unique_ptr<Node> root;
};


template<typename mesh_type, typename float_type, typename index_type>
class Mesh {
public:
    struct Element {
        template<index_type vertex_num>
        inline constexpr Vector3<float_type> v() const
        { return mesh->template v<vertex_num>(index); }

        index_type          index;
        Vector3<float_type> center;
        const mesh_type*    mesh;
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

    Vector3<float_t> at(double t) const;

    bool is_intersects(const AABBox<float_t>& box) const;

    std::pair<float_t, float_t> intersects(const AABBox<float_t>& box) const;

    template<typename mesh_t>
    std::optional<float_t> intersects(
        const typename mesh_t::Element& triangle,
        float_t epsilon = std::numeric_limits<float_t>::epsilon()
    ) const;

    template<typename mesh_t, typename index_t>
    std::vector<float_t> intersects(
        Mesh<mesh_t, float_t, index_t>& mesh,
        float_t epsilon = std::numeric_limits<float_t>::epsilon()
    ) const;

    template<typename T>
    std::vector<float_t> intersects(
        const KDTree<T>& tree,
        float_t epsilon = std::numeric_limits<float_t>::epsilon()
    ) const;
private:
    template<typename T>
    void recursive_intersects(
        const typename KDTree<T>::Node& node,
        std::vector<float_t>& output,
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
inline constexpr Vector3<T>::Vector3(): coords({0, 0, 0}) {
}

template<typename T>
inline constexpr Vector3<T>::Vector3(T x, T y, T z): coords({x, y, z}) {
}

template<typename T>
inline Vector3<T> Vector3<T>::operator+(const Vector3<T>& rhs) const {
    return Vector3<T>(
        x() + rhs.x(),
        y() + rhs.y(),
        z() + rhs.z()
    );
}

template<typename T>
inline void Vector3<T>::operator+=(const Vector3<T>& rhs) {
    coords[0] += rhs.x();
    coords[1] += rhs.y();
    coords[2] += rhs.z();
}

template<typename T>
inline Vector3<T> Vector3<T>::operator-(const Vector3<T>& rhs) const {
    return Vector3<T>(x() - rhs.x(), y() - rhs.y(), z() - rhs.z());
}

template<typename T>
inline void Vector3<T>::operator-=(const Vector3<T>& rhs) {
    coords[0] -= rhs.x();
    coords[1] -= rhs.y();
    coords[2] -= rhs.z();
}

template<typename T>
inline Vector3<T> Vector3<T>::operator*(T value) const {
    return Vector3<T>(x() * value, y() * value, z() * value);
}

template<typename T>
inline Vector3<T> Vector3<T>::operator*(const Vector3<T>& rhs) const {
    return Vector3<T>(x() * rhs.x(), y() * rhs.y(), z() * rhs.z());
}

template<typename T>
inline Vector3<T> Vector3<T>::operator/(T value) const {
    return Vector3<T>(x() / value, y() / value, z() / value);
}

template<typename T>
inline bool Vector3<T>::operator==(const Vector3<T>& rhs) const {
    return x() == rhs.x() && y() == rhs.y() && z() == rhs.z();
}

template<typename T>
inline bool Vector3<T>::operator!=(const Vector3<T>& rhs) const {
    return x() != rhs.x() || y() != rhs.y() || z() != rhs.z();
}

template<typename T>
inline T Vector3<T>::length() const {
    return sqrt(x()*x() + y()*y() + z()*z());
}

template<typename T>
inline Vector3<T> Vector3<T>::cross(const Vector3<T>& rhs) const {
    return Vector3<T>(
        y() * rhs.z() - z() * rhs.y(),
        z() * rhs.x() - x() * rhs.z(),
        x() * rhs.y() - y() * rhs.x()
    );
}

template<typename T>
inline T Vector3<T>::dot(const Vector3<T>& rhs) const {
    return x() * rhs.x() + y() * rhs.y() + z() * rhs.z();
}

template<typename T>
inline Vector3<T> Vector3<T>::ort() const {
    T len = length();
    return Vector3<T>(x() / len, y() / len, z() / len);
}

template<typename T>
inline constexpr T Vector3<T>::x() const {
    return coords[0];
}

template<typename T>
inline constexpr T Vector3<T>::y() const {
    return coords[1];
}

template<typename T>
inline constexpr T Vector3<T>::z() const {
    return coords[2];
}

template<typename T>
inline T Vector3<T>::operator[](int index) const {
    return coords[index];
}


// AABBox implementation
template<typename T>
AABBox<typename T::float_t> get_bounding_box(const typename T::Element& element) {
    const Vector3 v1 = element.template v<0>();
    const Vector3 v2 = element.template v<1>();
    const Vector3 v3 = element.template v<2>();

    return {
        Vector3(
            std::min(v1.x(), std::min(v2.x(), v3.x())),
            std::min(v1.y(), std::min(v2.y(), v3.y())),
            std::min(v1.z(), std::min(v2.z(), v3.z()))
        ),
        Vector3(
            std::max(v1.x(), std::max(v2.x(), v3.x())),
            std::max(v1.y(), std::max(v2.y(), v3.y())),
            std::max(v1.z(), std::max(v2.z(), v3.z()))
        )
    };

}

template<typename T>
AABBox<typename T::float_t> get_bounding_box(const Range<typename T::iterator>& range) {
    AABBox<typename T::float_t> box;
    for (const auto& cur : range) {
        box += get_bounding_box<T>(cur);
    }
    return box;
}

template<typename T>
inline T AABBox<T>::volume() const {
    const auto dim = max - min;
    return dim.x() * dim.y() * dim.z();
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
AABBox<T> AABBox<T>::operator+(const AABBox<T>& box) const {
    return {
        Vector3(
            std::min(min.x(), box.min.x()),
            std::min(min.y(), box.min.y()),
            std::min(min.z(), box.min.z())
        ),
        Vector3(
            std::max(max.x(), box.max.x()),
            std::max(max.y(), box.max.y()),
            std::max(max.z(), box.max.z())
        )
    };
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
template<typename T>
std::pair<typename T::iterator, double> SAHSplitter<T>::find_min_sah(
    const Range<typename T::iterator>& range
) const {
    const auto length = range.length();

    std::vector<AABBox<typename T::float_t>> pref(length + 1);
    std::vector<AABBox<typename T::float_t>> suf(length + 1);

    typename T::iterator::difference_type i = 0;
    for (auto it = range.begin(), rit = std::prev(range.end()); it != range.end(); ++it, ++i, --rit) {
        pref[i + 1] = pref[i] + get_bounding_box<T>(*it);
        suf[i + 1] = suf[i] + get_bounding_box<T>(*rit);
    }

    auto mid = length;
    auto min_sah = length * pref[length].volume();
    for (i = 1; i < length; ++i) {
        const auto sah = i * pref[i].volume() + (length - i) * suf[length - i].volume();
        if (sah < min_sah) {
            min_sah = sah;
            mid = i;
        }
    }

    return std::make_pair(std::next(range.begin(), mid), min_sah);
}

template<typename T>
typename T::iterator SAHSplitter<T>::operator()(
    const Range<typename T::iterator>& range, int
) const {
    int splitting_axis = 0;
    double min_sah = std::numeric_limits<double>::max();
    auto split = range.end();
    for (int axis = 0; axis < 3; ++axis) {
        std::sort(range.begin(), range.end(), [axis](const auto& it1, const auto& it2) {
            return it1.center[axis] < it2.center[axis];
        });

        if (const auto [mid, sah] = find_min_sah(range); sah < min_sah) {
            min_sah = sah;
            splitting_axis = axis;
            split = mid;
        }
    }

    if (splitting_axis != 2) {
        std::sort(range.begin(), range.end(), [splitting_axis](const auto& it1, const auto& it2) {
            return it1.center[splitting_axis] < it2.center[splitting_axis];
        });
    }

    if (range.length() <= threshold) {
        return range.end();
    }
    return split;
}


template<typename T>
typename T::iterator MedianSplitter<T>::operator()(
    const Range<typename T::iterator>& range,
    int depth
) const {
    auto length = range.length();

    if (depth >= depth_limit || length <= 32) {
        return range.end();
    }

    int axis = depth % 3;
    std::sort(range.begin(), range.end(), [axis](const auto& it1, const auto& it2) {
        return it1.center[axis] < it2.center[axis];
    });

    return std::next(range.begin(), length / 2);
}


template<typename T>
template<typename Splitter>
std::unique_ptr<typename KDTree<T>::Node> KDTree<T>::Node::build(
    const Range<typename T::iterator>& range,
    int depth,
    const Splitter& splitter
) {
    auto split = splitter(range, depth);
    if (split == range.end()) {
        return std::make_unique<Node>(get_bounding_box<T>(range), range);
    }

    auto left = build(Range(range.begin(), split), depth + 1, splitter);
    auto right = build(Range(split, range.end()), depth + 1, splitter);

    return std::make_unique<Node>(
        left->box() + right->box(),
        range,
        std::move(left), std::move(right)
    );
}


// Ray implementation
template<typename float_t>
inline Vector3<float_t> Ray<float_t>::at(double t) const {
    return Vector3<float_t>(
        origin.x() + vector.x() * t,
        origin.y() + vector.y() * t,
        origin.z() + vector.z() * t
    );
}

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

/*
 * Read for details:
 * https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
 */
template<typename float_t>
template<typename mesh_t>
std::optional<float_t> Ray<float_t>::intersects(
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

    return t;
}


template<typename float_t>
template<typename T>
void Ray<float_t>::recursive_intersects(
    const typename KDTree<T>::Node& node,
    std::vector<float_t>& output,
    float_t epsilon
) const {
    if (node.is_leaf()) {
        for (const auto& triangle : node.triangles()) {
            if (auto intersection = intersects<T>(triangle, epsilon); intersection) {
                output.push_back(*intersection);
            }
        }
    } else {
        if (node.has_left() && is_intersects(node.left().box())) {
            recursive_intersects<T>(node.left(), output, epsilon);
        }
        if (node.has_right() && is_intersects(node.right().box())) {
            recursive_intersects<T>(node.right(), output, epsilon);
        }
    }
}


template<typename float_t>
template<typename T>
std::vector<float_t> Ray<float_t>::intersects(
    const KDTree<T>& tree,
    float_t epsilon
) const {
    const auto& node = tree.top();
    if (!is_intersects(node.box())) {
        return {};
    }

    std::vector<float_t> output;
    recursive_intersects<T>(node, output, epsilon);
    return output;
}


/*
 * Simple iterative intersection search
 */
template<typename float_t>
template<typename mesh_t, typename index_t>
std::vector<float_t> Ray<float_t>::intersects(
    Mesh<mesh_t, float_t, index_t>& mesh,
    float_t epsilon
) const {
    std::vector<float_t> intersections;

    for (const auto& cur : mesh) {
        if (auto intersection = intersects<mesh_t>(cur, epsilon); intersection) {
            intersections.push_back(*intersection);
        }
    }

    return intersections;
}

template<typename float_t>
inline std::pair<float_t, float_t> Ray<float_t>::intersects(const AABBox<float_t>& box) const {
    Vector3 t1 = (box.min - origin) * inv_vector;
    Vector3 t2 = (box.max - origin) * inv_vector;

    return std::make_pair(
        std::max({
            std::min(t1.x(), t2.x()),
            std::min(t1.y(), t2.y()),
            std::min(t1.z(), t2.z())
        }),
        std::min({
            std::max(t1.x(), t2.x()),
            std::max(t1.y(), t2.y()),
            std::max(t1.z(), t2.z())
        })
    );
}

template<typename float_t>
inline bool Ray<float_t>::is_intersects(const AABBox<float_t>& box) const {
    auto [tmin, tmax] = intersects(box);
    return tmax >= 0.0 && tmin <= tmax;
}


} // namespace rmi

