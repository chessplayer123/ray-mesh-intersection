#pragma once

#ifdef RMI_INCLUDE_POOL
#    include "thread_pool.hpp"

template<typename T>
std::vector<Vector> parallel_intersects_pool(
    const Ray& ray,
    const KDTree<T>& tree,
    int threads_count,
    double epsilon = std::numeric_limits<double>::epsilon()
) {
    auto root = tree.top();
    if (!ray.intersects(root.box())) {
        return {};
    }

    ThreadPool<T> pool(ray, root, threads_count);
    return pool.wait_result();
}

#endif // RMI_INCLUDE_POOL


#ifdef RMI_INCLUDE_OMP
#    include <omp.h>
#    include <vector>
#    include "vector.hpp"
#    include "kd_tree.hpp"
#    include "ray.hpp"

template<typename T>
void parallel_recursive_intersects(
    const Ray& ray,
    typename KDTree<T>::iterator iter,
    std::vector<Vector>& output,
    double epsilon
) {
    if (iter.is_leaf()) {
        auto [begin, end] = iter.triangles();
        for (auto cur = begin; cur != end; ++cur) {
            auto intersection = ray.intersects<T>(*cur, epsilon);
            if (intersection.has_value()) {
                output.push_back(intersection.value());
            }
        }
    } else {
        auto left = iter.left();
        auto right = iter.right();
        bool intersects_left = ray.intersects(left.box());
        bool intersects_right = ray.intersects(right.box());

        if (intersects_left && intersects_right) {
            #pragma omp task shared(output)
            parallel_recursive_intersects<T>(ray, left, output, epsilon);

            parallel_recursive_intersects<T>(ray, right, output, epsilon);
            #pragma omp taskwait
        } else if (intersects_left) {
            parallel_recursive_intersects<T>(ray, left, output, epsilon);
        } else if (intersects_right) {
            parallel_recursive_intersects<T>(ray, right, output, epsilon);
        }
    }
}


template<typename T>
std::vector<Vector> parallel_intersects_omp(
    const Ray& ray,
    const KDTree<T>& tree,
    int threads_count,
    double epsilon = std::numeric_limits<double>::epsilon()
) {
    auto iter = tree.top();
    if (!ray.intersects(iter.box())) {
        return {};
    }

    std::vector<Vector> output;

    #pragma omp parallel shared(output) num_threads(threads_count)
    #pragma omp single
    parallel_recursive_intersects<T>(ray, iter, output, epsilon);

    return output;
}

#endif // RMI_INCLUDE_OMP
