#pragma once


#include <iostream>
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
    T       operator[](int index) const;

    bool        equals(const Vector3& rhs, double epsilon) const;
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
class Subrange : public Range<T> {
public:
    Subrange(T begin, T end, bool splittable):
        Range<T>(begin, end),
        splittable(splittable)
    {}

    inline bool is_splittable() const { return splittable; }
private:
    bool splittable;
};


template<typename T, int N>
struct SAHSplitter {
    SAHSplitter(int threshold = 16): threshold(threshold) {}

    std::vector<Subrange<typename T::iterator>> operator()(
        const Range<typename T::iterator>& range,
        int depth
    ) const;

    std::pair<typename T::iterator, double> find_min_sah(
        const Range<typename T::iterator>& range
    ) const;

    int threshold;
};


template<typename T, int N>
struct MidSplitter {
    MidSplitter(int depth_limit = 16): depth_limit(depth_limit) {}

    std::vector<Subrange<typename T::iterator>> operator()(
        const Range<typename T::iterator>& range,
        int depth
    ) const;

    int depth_limit;
};


template<typename T, int N>
class Tree {
public:
    typedef typename T::iterator mesh_iterator;

    class Node {
    public:
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

        Node(
            AABBox<typename T::float_t> box,
            Range<mesh_iterator> range
        ): bounding_box(box), range(range), children() {}

        template<typename Splitter>
        static Node build(Range<typename T::iterator> range, int depth, const Splitter& splitter);

        inline bool                               is_leaf()     const { return children.empty(); }
        inline const std::vector<Node>&           child_nodes() const { return children; }
        inline const AABBox<typename T::float_t>& box()         const { return bounding_box; }
        inline const Range<mesh_iterator>&        triangles()   const { return range; }
    private:
        AABBox<typename T::float_t> bounding_box;
        Range<mesh_iterator> range;
        std::vector<Node> children;
    };

    Tree(Node&& root): root(std::move(root)) {}

    template<typename Splitter = SAHSplitter<T, N>>
    inline static Tree<T, N> for_mesh(
        mesh_iterator begin,
        mesh_iterator end,
        const Splitter& splitter = Splitter()
    ) {
        return Tree<T, N>(Node::build(Range(begin, end), 0, splitter));
    }

    inline const Node& top() const
    { return root; }
private:
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
inline constexpr Vector3<T>::Vector3(): coords({0, 0, 0}) {
}

template<typename T>
inline constexpr Vector3<T>::Vector3(T x, T y, T z): coords({x, y, z}) {
}

template<typename T>
inline std::ostream& operator<<(std::ostream& stream, const Vector3<T>& vec) {
    return stream << "Vector(" << vec.x() << ", " << vec.y() << ", " << vec.z() << ")";
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
inline bool Vector3<T>::equals(const Vector3<T>& rhs, double epsilon) const {
    return abs(x() - rhs.x()) < epsilon
        && abs(y() - rhs.y()) < epsilon
        && abs(z() - rhs.z()) < epsilon;
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
    using std::min, std::max;

    const Vector3 v1 = element.template v<0>();
    const Vector3 v2 = element.template v<1>();
    const Vector3 v3 = element.template v<2>();

    return {
        Vector3(
            min(v1.x(), min(v2.x(), v3.x())),
            min(v1.y(), min(v2.y(), v3.y())),
            min(v1.z(), min(v2.z(), v3.z()))
        ),
        Vector3(
            max(v1.x(), max(v2.x(), v3.x())),
            max(v1.y(), max(v2.y(), v3.y())),
            max(v1.z(), max(v2.z(), v3.z()))
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
template<typename T, int N>
std::pair<typename T::iterator, double> SAHSplitter<T, N>::find_min_sah(
    const Range<typename T::iterator>& range
) const {
    const auto length = range.length();

    std::vector<AABBox<typename T::float_t>> pref(length + 1);
    std::vector<AABBox<typename T::float_t>> suf(length + 1);

    typename T::iterator::difference_type i = 0;
    for (auto it = range.begin(), rit = range.end() - 1; it != range.end(); ++it, ++i, --rit) {
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

template<typename T, int N>
std::vector<Subrange<typename T::iterator>> SAHSplitter<T, N>::operator()(
    const Range<typename T::iterator>& range, int
) const {
    std::vector<Subrange<typename T::iterator>> subranges;
    std::vector<std::pair<Range<typename T::iterator>, int>> unsplitted {{range, N}};

    while (!unsplitted.empty()) {
        auto [cur, splits_remained] = unsplitted.back();
        unsplitted.pop_back();

        int splitting_axis = 0;
        double min_sah = std::numeric_limits<double>::max();
        auto split = cur.end();
        for (int axis = 0; axis < 3; ++axis) {
            std::sort(cur.begin(), cur.end(), [axis](const auto& it1, const auto& it2) {
                return it1.center[axis] < it2.center[axis];
            });

            if (const auto [mid, sah] = find_min_sah(cur); sah < min_sah) {
                min_sah = sah;
                splitting_axis = axis;
                split = mid;
            }
        }

        if (splitting_axis != 2) {
            std::sort(cur.begin(), cur.end(), [splitting_axis](const auto& it1, const auto& it2) {
                return it1.center[splitting_axis] < it2.center[splitting_axis];
            });
        }

        if (split == cur.end() || cur.length() <= threshold) {
            subranges.emplace_back(cur.begin(), cur.end(), false);
        } else if (splits_remained == 1) {
            subranges.emplace_back(cur.begin(), split, true);
            subranges.emplace_back(split, cur.end(), true);
        } else {
            unsplitted.emplace_back(Range(cur.begin(), split), splits_remained - 1);
            unsplitted.emplace_back(Range(split, cur.end()), splits_remained - 1);
        }
    }

    return subranges;
}


template<typename T, int N>
std::vector<Subrange<typename T::iterator>> MidSplitter<T, N>::operator()(
    const Range<typename T::iterator>& range,
    int depth
) const {
    std::vector<Subrange<typename T::iterator>> subranges;
    std::vector<std::pair<Range<typename T::iterator>, int>> unsplitted {{range, N}};

    while (!unsplitted.empty()) {
        auto [cur, splits_remained] = unsplitted.back();
        unsplitted.pop_back();

        auto length = cur.length();
        if (depth >= depth_limit || length <= (1 << N)) {
            subranges.emplace_back(cur.begin(), cur.end(), false);
            continue;
        }

        int axis = (depth + splits_remained) % 3;
        std::sort(cur.begin(), cur.end(), [axis](const auto& it1, const auto& it2) {
            return it1.center[axis] < it2.center[axis];
        });

        auto mid = std::next(cur.begin(), length / 2);
        if (splits_remained == 1) {
            subranges.emplace_back(cur.begin(), mid, true);
            subranges.emplace_back(mid, cur.end(), true);
        } else {
            unsplitted.emplace_back(Range(cur.begin(), mid), splits_remained - 1);
            unsplitted.emplace_back(Range(mid, cur.end()), splits_remained - 1);
        }
    }

    return subranges;
}


template<typename T, int N>
template<typename Splitter>
typename Tree<T, N>::Node Tree<T, N>::Node::build(
    Range<typename T::iterator> range,
    int depth,
    const Splitter& splitter
) {
    auto subranges = splitter(range, depth);
    std::vector<typename Tree<T, N>::Node> children;
    AABBox<typename T::float_t> aabb;
    for (const auto& subrange : subranges) {
        auto child = subrange.is_splittable() ?
            build(subrange, depth + 1, splitter) :
            Node(get_bounding_box<T>(subrange), subrange);

        aabb += child.box();
        children.push_back(std::move(child));
    }

    return Node(aabb, range, std::move(children));
}


// Ray implementation
template<typename float_t>
inline Vector3<float_t> Ray<float_t>::at(double t) const {
    return origin + vector * t;
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

    return at(t);
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

