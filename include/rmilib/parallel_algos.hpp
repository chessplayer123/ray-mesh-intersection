#pragma once

#ifdef RMI_INCLUDE_POOL
#    include "thread_pool.hpp"

template<typename T, int N>
std::vector<Vector3<typename T::float_t>> parallel_intersects_pool(
    const Ray<typename T::float_t>& ray,
    const Tree<T, N>& tree,
    int threads_count
) {
    const auto& root = tree.top();
    if (!ray.intersects(root.box())) {
        return {};
    }

    ThreadPool<T, N> pool(ray, &root, threads_count);
    return pool.wait_result();
}

#endif // RMI_INCLUDE_POOL


#ifdef RMI_INCLUDE_OMP
#    include <omp.h>
#    include <vector>
#    include "vector.hpp"
#    include "tree.hpp"
#    include "ray.hpp"

template<typename T, int N>
void parallel_recursive_intersects(
    const Ray<typename T::float_t>& ray,
    const typename Tree<T, N>::Node* node,
    std::vector<Vector3<typename T::float_t>>& output,
    double epsilon
) {
    if (node->is_leaf()) {
        for (const auto& cur : node->triangles()) {
            auto intersection = ray.template intersects<T>(cur, epsilon);
            if (intersection.has_value()) {
                output.push_back(intersection.value());
            }
        }
    } else {
        for (const auto& child : node->child_nodes()) {
            if (ray.intersects(child.box())) {
                const auto* child_ptr = &child;
                #pragma omp task shared(output)
                parallel_recursive_intersects<T, N>(ray, child_ptr, output, epsilon);
            }
        }
        #pragma omp taskwait
    }
}


template<typename T, int N>
std::vector<Vector3<typename T::float_t>> parallel_intersects_omp(
    const Ray<typename T::float_t>& ray,
    const Tree<T, N>& tree,
    int threads_count,
    double epsilon = std::numeric_limits<double>::epsilon()
) {
    const auto& root = tree.top();
    if (!ray.intersects(root.box())) {
        return {};
    }

    std::vector<Vector3<typename T::float_t>> output;

    #pragma omp parallel shared(output) num_threads(threads_count)
    #pragma omp single
    parallel_recursive_intersects<T, N>(ray, &root, output, epsilon);

    return output;
}

#endif // RMI_INCLUDE_OMP
