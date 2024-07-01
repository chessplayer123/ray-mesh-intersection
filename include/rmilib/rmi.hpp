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

#ifdef RMI_INCLUDE_POOL
#    include "wsq.hpp"
#    include <thread>
#endif

#ifdef RMI_INCLUDE_OMP
#    include <omp.h>
#endif


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

    T       length() const;
    T       dot(const Vector3& rhs) const;
    Vector3 cross(const Vector3& rhs) const;
    Vector3 ort() const;

    inline T x() const { return coords[0]; }
    inline T y() const { return coords[1]; }
    inline T z() const { return coords[2]; }

    inline void set_x(double x) { coords[0] = x; }
    inline void set_y(double y) { coords[1] = y; }
    inline void set_z(double z) { coords[2] = z; }
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
struct SAHSplitter {
    SAHSplitter(int threshold = 16): threshold(threshold) {}

    typename T::iterator operator()(
        typename T::iterator begin,
        typename T::iterator end,
        int depth
    ) const;

    std::pair<typename T::iterator, typename T::float_t> find_min_sah(
        typename T::iterator begin,
        typename T::iterator end
    ) const;

    int threshold;
};


template<typename T>
struct MedianSplitter {
    MedianSplitter(int depth_limit = 16): depth_limit(depth_limit) {}

    typename T::iterator operator()(
        typename T::iterator begin,
        typename T::iterator end,
        int depth
    ) const;

    int depth_limit;
};


template<typename T>
class Mesh {
public:
    struct Element {
        Element(
            Vector3<typename T::float_t> v1,
            Vector3<typename T::float_t> v2,
            Vector3<typename T::float_t> v3
        ): v1(v1), v2(v2), v3(v3), center((v1 + v2 + v3) / 3)
        {}

        Vector3<typename T::float_t> v1, v2, v3, center;
    };

    using iterator = typename std::vector<Element>::iterator;

    inline iterator begin() { return elements.begin(); }
    inline iterator end()   { return elements.end(); }

    void setup(typename std::vector<Element>::size_type size);
private:
    std::vector<Element> elements;
};


template<typename T>
class KDTree {
public:
    using mesh_iterator = typename T::iterator;

    class Node {
    public:
        friend KDTree;

        Node(
            AABBox<typename T::float_t> box,
            std::unique_ptr<Node>&& left,
            std::unique_ptr<Node>&& right
        ): bounding_box(box), m_begin(left->begin()), m_end(right->end()), m_left(std::move(left)), m_right(std::move(right)) {}

        Node(
            AABBox<typename T::float_t> box,
            mesh_iterator begin,
            mesh_iterator end
        ): bounding_box(box), m_begin(begin), m_end(end), m_left(nullptr), m_right(nullptr) {}

        inline bool                               is_leaf() const { return !m_left && !m_right; }
        inline const Node&                        left()    const { return *m_left; }
        inline const Node&                        right()   const { return *m_right; }
        inline const AABBox<typename T::float_t>& box()     const { return bounding_box; }
        inline mesh_iterator                      begin()   const { return m_begin; }
        inline mesh_iterator                      end()     const { return m_end; }
    private:
        template<typename Splitter>
        static std::unique_ptr<Node> build(
            mesh_iterator begin,
            mesh_iterator end,
            int depth,
            const Splitter& splitter
        );

#ifdef RMI_INCLUDE_OMP
        template<typename Splitter>
        static std::unique_ptr<Node> omp_build(
            mesh_iterator begin,
            mesh_iterator end,
            int depth,
            const Splitter& splitter
        );
#endif

        AABBox<typename T::float_t> bounding_box;

        mesh_iterator               m_begin;
        mesh_iterator               m_end;

        std::unique_ptr<Node>       m_left;
        std::unique_ptr<Node>       m_right;
    };

    KDTree(std::unique_ptr<Node>&& root): root(std::move(root)) {}

    template<typename Splitter = SAHSplitter<T>>
    static KDTree<T> for_mesh(Mesh<T>& mesh, const Splitter& splitter = Splitter());

#ifdef RMI_INCLUDE_OMP
    template<typename Splitter = SAHSplitter<T>>
    static KDTree<T> for_mesh(Mesh<T>& mesh, int threads_count, const Splitter& splitter = Splitter());
#endif

    inline const Node& top() const {
        return *root;
    }
private:
    std::unique_ptr<Node> root;
};


template<typename float_t>
class Ray {
public:
    Ray(Vector3<float_t> origin, Vector3<float_t> direction);

    Vector3<float_t> at(float_t t) const;

    bool is_intersects(const AABBox<float_t>& box) const;

    std::pair<float_t, float_t> intersects(const AABBox<float_t>& box) const;

    template<typename T>
    std::optional<Vector3<float_t>> intersects(
        const typename Mesh<T>::Element& triangle,
        float_t epsilon = std::numeric_limits<float_t>::epsilon()
    ) const;

    template<typename T>
    std::vector<Vector3<float_t>> intersects(
        Mesh<T>& mesh,
        float_t epsilon = std::numeric_limits<float_t>::epsilon()
    ) const;

    template<typename T>
    std::vector<Vector3<float_t>> intersects(
        const KDTree<T>& tree,
        float_t epsilon = std::numeric_limits<float_t>::epsilon()
    ) const;

#ifdef RMI_INCLUDE_POOL
    template<typename T>
    std::vector<Vector3<float_t>> pool_intersects(const KDTree<T>& tree, int threads_count) const;
#endif

#ifdef RMI_INCLUDE_OMP
    template<typename T>
    std::vector<Vector3<float_t>> omp_intersects(
        const KDTree<T>& tree,
        int threads_count,
        float_t epsilon = std::numeric_limits<float_t>::epsilon()
    ) const;

    template<typename T>
    std::vector<Vector3<float_t>> omp_intersects(
        Mesh<T>& mesh,
        int threads_count,
        float_t epsilon = std::numeric_limits<float_t>::epsilon()
    ) const;
#endif

private:
    template<typename T>
    void recursive_intersects(
        const typename KDTree<T>::Node& node,
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
inline T Vector3<T>::operator[](int index) const {
    return coords[index];
}


// AABBox implementation
template<typename T>
AABBox<typename T::float_t> get_bounding_box(const typename T::Element& element) {
    const Vector3 v1 = element.v1;
    const Vector3 v2 = element.v2;
    const Vector3 v3 = element.v3;

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
AABBox<typename T::float_t> get_bounding_box(
    typename T::iterator begin,
    typename T::iterator end
) {
    AABBox<typename T::float_t> box;
    for (auto it = begin; it != end; ++it) {
        box += get_bounding_box<T>(*it);
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
template<typename T>
void Mesh<T>::setup(typename std::vector<typename Mesh<T>::Element>::size_type size) {
    auto mesh = static_cast<T*>(this);
    elements.clear();
    elements.reserve(size);
    for (typename T::index_t i = 0; i < size; ++i) {
        elements.emplace_back(
            mesh->template v<0>(i),
            mesh->template v<1>(i),
            mesh->template v<2>(i)
        );
    }
}


// Tree implementation

template<typename T>
template<typename Splitter>
inline KDTree<T> KDTree<T>::for_mesh(
    Mesh<T>& mesh,
    const Splitter& splitter
) {
    return KDTree<T>(Node::build(mesh.begin(), mesh.end(), 0, splitter));
}


#ifdef RMI_INCLUDE_OMP
template<typename T>
template<typename Splitter>
inline KDTree<T> KDTree<T>::for_mesh(
    Mesh<T>& mesh,
    int threads_count,
    const Splitter& splitter
) {
    std::unique_ptr<typename KDTree<T>::Node> node;
    #pragma omp parallel num_threads(threads_count) shared(node, mesh)
    #pragma omp single
    node = std::move(Node::omp_build(mesh.begin(), mesh.end(), 0, splitter));

    return KDTree<T>(std::move(node));
}
#endif


template<typename T>
std::pair<typename T::iterator, typename T::float_t> SAHSplitter<T>::find_min_sah(
    typename T::iterator begin,
    typename T::iterator end
) const {
    const auto length = std::distance(begin, end);

    std::vector<AABBox<typename T::float_t>> pref(length + 1);
    std::vector<AABBox<typename T::float_t>> suf(length + 1);

    typename T::iterator::difference_type i = 0;
    for (auto it = begin, rit = std::prev(end); it != end; ++it, ++i, --rit) {
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

    return std::make_pair(
        std::next(begin, mid),
        min_sah
    );
}

template<typename T>
typename T::iterator SAHSplitter<T>::operator()(
    typename T::iterator begin,
    typename T::iterator end,
    int
) const {
    if (std::distance(begin, end) <= threshold) {
        return end;
    }

    int splitting_axis = 0;
    auto min_sah = std::numeric_limits<typename T::float_t>::max();
    auto split = end;
    for (int axis = 0; axis < 3; ++axis) {
        std::sort(begin, end, [axis](const auto& it1, const auto& it2) {
            return it1.center[axis] < it2.center[axis];
        });

        if (const auto [mid, sah] = find_min_sah(begin, end); sah < min_sah) {
            min_sah = sah;
            splitting_axis = axis;
            split = mid;
        }
    }

    if (splitting_axis != 2) {
        std::sort(begin, end, [splitting_axis](const auto& it1, const auto& it2) {
            return it1.center[splitting_axis] < it2.center[splitting_axis];
        });
    }
    return split;
}


template<typename T>
typename T::iterator MedianSplitter<T>::operator()(
    typename T::iterator begin,
    typename T::iterator end,
    int depth
) const {
    auto length = std::distance(begin, end);

    if (depth >= depth_limit || length <= 32) {
        return end;
    }

    int axis = depth % 3;
    std::sort(begin, end, [axis](const auto& it1, const auto& it2) {
        return it1.center[axis] < it2.center[axis];
    });

    return std::next(begin, length / 2);
}


template<typename T>
template<typename Splitter>
std::unique_ptr<typename KDTree<T>::Node> KDTree<T>::Node::build(
    typename T::iterator begin,
    typename T::iterator end,
    int depth,
    const Splitter& splitter
) {
    auto split = splitter(begin, end, depth);
    if (split == end) {
        return std::make_unique<Node>(get_bounding_box<T>(begin, end), begin, end);
    }

    auto left = build(begin, split, depth + 1, splitter);
    auto right = build(split, end, depth + 1, splitter);

    return std::make_unique<Node>(
        left->box() + right->box(),
        std::move(left), std::move(right)
    );
}

#ifdef RMI_INCLUDE_OMP
template<typename T>
template<typename Splitter>
std::unique_ptr<typename KDTree<T>::Node> KDTree<T>::Node::omp_build(
    typename T::iterator begin,
    typename T::iterator end,
    int depth,
    const Splitter& splitter
) {
    auto split = splitter(begin, end, depth);
    if (split == end) {
        return std::make_unique<typename KDTree<T>::Node>(get_bounding_box<T>(begin, end), begin, end);
    }
    std::unique_ptr<typename KDTree<T>::Node> left;
    std::unique_ptr<typename KDTree<T>::Node> right;

    #pragma omp task shared(left)
    left = omp_build(begin, split, depth + 1, splitter);
    #pragma omp task shared(right)
    right = omp_build(split, end, depth + 1, splitter);
    #pragma omp taskwait

    return std::make_unique<typename KDTree<T>::Node>(
        left->box() + right->box(),
        std::move(left), std::move(right)
    );
}
#endif


// Ray implementation
template<typename float_t>
inline Vector3<float_t> Ray<float_t>::at(float_t t) const {
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
template<typename T>
std::optional<Vector3<float_t>> Ray<float_t>::intersects(
    const typename Mesh<T>::Element& triangle,
    float_t epsilon
) const {
    Vector3 edge1 = triangle.v2 - triangle.v1;
    Vector3 edge2 = triangle.v3 - triangle.v1;
    Vector3 ray_cross_e2 = vector.cross(edge2);

    float_t det = edge1.dot(ray_cross_e2);
    if (-epsilon <= det && det <= epsilon) {
        return std::nullopt;
    }
    float_t inv_det = 1.0 / det;

    Vector3 s = origin - triangle.v1;
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
template<typename T>
void Ray<float_t>::recursive_intersects(
    const typename KDTree<T>::Node& node,
    std::vector<Vector3<float_t>>& output,
    float_t epsilon
) const {
    if (node.is_leaf()) {
        for (const auto& triangle : node) {
            if (auto intersection = intersects<T>(triangle, epsilon); intersection) {
                output.push_back(std::move(*intersection));
            }
        }
    } else {
        if (is_intersects(node.left().box())) {
            recursive_intersects<T>(node.left(), output, epsilon);
        }
        if (is_intersects(node.right().box())) {
            recursive_intersects<T>(node.right(), output, epsilon);
        }
    }
}


template<typename float_t>
template<typename T>
std::vector<Vector3<float_t>> Ray<float_t>::intersects(
    const KDTree<T>& tree,
    float_t epsilon
) const {
    const auto& node = tree.top();
    if (!is_intersects(node.box())) {
        return {};
    }

    std::vector<Vector3<float_t>> output;
    recursive_intersects<T>(node, output, epsilon);
    return output;
}


/*
 * Simple iterative intersection search
 */
template<typename float_t>
template<typename T>
std::vector<Vector3<float_t>> Ray<float_t>::intersects(
    Mesh<T>& mesh,
    float_t epsilon
) const {
    std::vector<Vector3<float_t>> intersections;

    for (const auto& cur : mesh) {
        if (auto intersection = intersects<T>(cur, epsilon); intersection) {
            intersections.push_back(std::move(*intersection));
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

#ifdef RMI_INCLUDE_POOL
namespace parallel {

template<typename T>
class ThreadPool {
public:
    using Node = typename KDTree<T>::Node;
    using float_t = typename T::float_t;
    using index_t = typename T::index_t;

    ThreadPool(
        const Ray<float_t>& ray,
        const Node* root,
        int threads_count
    ):
        threads(threads_count), queues(threads_count), results(threads_count),
        counter(0), threads_count(threads_count), ray(ray)
    {
        distribute_load(root);

        for (int i = 0; i < threads_count; ++i) {
            threads[i] = std::thread(&ThreadPool::worker_thread, this, i);
        }
    }

    std::vector<Vector3<float_t>> wait_result() {
        std::vector<Vector3<float_t>> result;
        for (int id = 0; id < threads_count; ++id) {
            threads[id].join();
            std::copy(results[id].begin(), results[id].end(), std::back_inserter(result));
        }
        return result;
    }
private:
    void distribute_load(const Node* root) {
        queues[0].push(root);
    }

    std::optional<const Node*> pop_node(int thread_id) {
        auto node = queues[thread_id].pop();
        if (node) {
            return node;
        }

        ++counter;
        while (counter < threads_count) {
            for (auto& queue : queues) {
                auto steal = queue.steal();
                if (steal) {
                    --counter;
                    return steal;
                }
            }
        }
        return std::nullopt;
    }

    void worker_thread(int thread_id) {
        std::optional<const Node*> next = pop_node(thread_id);

        while(next) {
            auto cur = *next;

            if (cur->is_leaf()) {
                for (const auto& triangle : *cur) {
                    if (auto intersection = ray.template intersects<T>(triangle); intersection) {
                        results[thread_id].push_back(std::move(*intersection));
                    }
                }
                next = pop_node(thread_id);
            } else {
                bool intersects_left  = ray.is_intersects(cur->left().box());
                bool intersects_right = ray.is_intersects(cur->right().box());

                switch ((intersects_left << 1) | intersects_right) {
                case 0b00: next = pop_node(thread_id); break;
                case 0b01: next = &cur->right(); break;
                case 0b10: next = &cur->left(); break;
                case 0b11:
                    next = &cur->left();
                    queues[thread_id].push(&cur->right());
                    break;
                }
            }
        }
    }

    std::vector<std::thread> threads;
    std::vector<WorkStealingQueue<const Node*>> queues;
    std::vector<std::vector<Vector3<float_t>>> results;

    std::atomic_int counter;
    int threads_count;

    const Ray<float_t>& ray;
};

} // namespace parallel


template<typename float_t>
template<typename T>
std::vector<Vector3<float_t>> Ray<float_t>::pool_intersects(
    const KDTree<T>& tree,
    int threads_count
) const {
    const auto& root = tree.top();
    if (!is_intersects(root.box())) {
        return {};
    }

    parallel::ThreadPool<T> pool(*this, &root, threads_count);
    return pool.wait_result();
}

#endif


#ifdef RMI_INCLUDE_OMP

template<typename float_t>
template<typename T>
std::vector<Vector3<float_t>> Ray<float_t>::omp_intersects(
    Mesh<T>& mesh,
    int threads_count,
    float_t epsilon
) const {
    std::vector<Vector3<float_t>> intersections;

    #pragma omp parallel for shared(intersections) num_threads(threads_count)
    for (const auto& triangle : mesh) {
        if (auto intersection = intersects<T>(triangle, epsilon); intersection) {
            #pragma omp critical
            intersections.push_back(std::move(*intersection));
        }
    }

    return intersections;
}

template<typename T>
void omp_recursive_intersects(
    const Ray<typename T::float_t>& ray,
    const typename KDTree<T>::Node& node,
    std::vector<Vector3<typename T::float_t>>& output,
    double epsilon
) {
    if (node.is_leaf()) {
        for (const auto& cur : node) {
            if (auto intersection = ray.template intersects<T>(cur, epsilon); intersection) {
                #pragma omp critical
                output.push_back(std::move(*intersection));
            }
        }
    } else {
        if (ray.is_intersects(node.left().box())) {
            #pragma omp task shared(output, node)
            omp_recursive_intersects<T>(ray, node.left(), output, epsilon);
        }

        if (ray.is_intersects(node.right().box())) {
            #pragma omp task shared(output, node)
            omp_recursive_intersects<T>(ray, node.right(), output, epsilon);
        }
    }
}


template<typename float_t>
template<typename T>
std::vector<Vector3<float_t>> Ray<float_t>::omp_intersects(
    const KDTree<T>& tree,
    int threads_count,
    float_t epsilon
) const {
    const auto& root = tree.top();
    if (!is_intersects(root.box())) {
        return {};
    }

    std::vector<Vector3<float_t>> output;

    #pragma omp parallel shared(output, root) num_threads(threads_count)
    #pragma omp single
    omp_recursive_intersects<T>(*this, root, output, epsilon);
    #pragma omp taskwait

    return output;
}

#endif // RMI_INCLUDE_OMP


} // namespace rmi

