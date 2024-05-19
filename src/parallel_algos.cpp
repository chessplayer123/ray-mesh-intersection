#include "parallel_algos.hpp"


#if !defined(RMI_BACKEND) || RMI_BACKEND == RMI_BACKEND_NONE
#    pragma message("Using sync algo instead of parallel")

std::vector<Vector> parallel_intersects(
    const Ray& ray,
    const KDTree& tree,
    int threads_count,
    double epsilon
) {
    return ray.intersects(tree, epsilon);
}

#elif RMI_BACKEND == RMI_BACKEND_POOL
#    include "thread_pool.hpp"

std::vector<Vector> parallel_intersects(
    const Ray& ray,
    const KDTree& tree,
    int threads_count,
    double epsilon
) {
    auto root = tree.iterator();
    if (!ray.intersects(root.box())) {
        return {};
    }

    ThreadPool pool(ray, tree.iterator(), threads_count);
    return pool.wait_result();
}

#elif RMI_BACKEND == RMI_BACKEND_OPENMP
#    include <omp.h>

void parallel_recursive_intersects(
    const Ray& ray,
    KDTree::Iterator iter,
    std::vector<Vector>& output,
    double epsilon
) {
    if (iter.is_leaf()) {
        for (const Triangle& triangle : iter.triangles()) {
            auto intersection = ray.intersects(triangle, epsilon);
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
            parallel_recursive_intersects(ray, left, output, epsilon);

            parallel_recursive_intersects(ray, right, output, epsilon);
            #pragma omp taskwait
        } else if (intersects_left) {
            parallel_recursive_intersects(ray, left, output, epsilon);
        } else if (intersects_right) {
            parallel_recursive_intersects(ray, right, output, epsilon);
        }
    }
}


std::vector<Vector> parallel_intersects(
    const Ray& ray,
    const KDTree& tree,
    int threads_count,
    double epsilon
) {
    KDTree::Iterator iterator = tree.iterator();
    if (!ray.intersects(iterator.box())) {
        return {};
    }

    std::vector<Vector> output;

    #pragma omp parallel shared(output) num_threads(threads_count)
    #pragma omp single
    parallel_recursive_intersects(ray, iterator, output, epsilon);

    return output;
}

#else
#    error Backend is not supported
#endif
