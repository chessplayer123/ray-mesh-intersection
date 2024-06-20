#pragma once


#ifdef RMI_INCLUDE_POOL
#    include "rmi.hpp"
#    include "wsq.hpp"
#    include <thread>
namespace rmi::parallel {

template<typename T>
std::vector<Vector3<typename T::float_t>> pool_intersects(
    const Ray<typename T::float_t>& ray,
    const KDTree<T>& tree,
    int threads_count
);

}
#endif // RMI_INCLUDE_POOL


#ifdef RMI_INCLUDE_OMP
#    include "rmi.hpp"
#    include <vector>
#    include <omp.h>
namespace rmi::parallel {

template<typename T>
std::vector<Vector3<typename T::float_t>> omp_intersects(
    const Ray<typename T::float_t>& ray,
    const KDTree<T>& tree,
    int threads_count,
    typename T::float_t epsilon = std::numeric_limits<typename T::float_t>::epsilon()
);


template<typename mesh_t, typename float_t, typename index_t>
std::vector<Vector3<float_t>> omp_intersects(
    const Ray<float_t>& ray,
    Mesh<mesh_t, float_t, index_t>& mesh,
    int threads_count,
    float_t epsilon = std::numeric_limits<float_t>::epsilon()
);


template<typename T, typename Splitter = rmi::SAHSplitter<T>>
KDTree<T> omp_build(
    typename T::iterator begin,
    typename T::iterator end,
    int threads_count,
    const Splitter& splitter = Splitter()
);

}
#endif // RMI_INCLUDE_OMP



#ifdef RMI_INCLUDE_POOL
namespace rmi::parallel {

template<typename T>
class ThreadPool {
public:
    typedef typename KDTree<T>::Node Node;
    typedef typename T::float_t float_t;
    typedef typename T::index_t index_t;

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
            const Node* cur = *next;

            if (cur->is_leaf()) {
                for (const auto& triangle : cur->triangles()) {
                    if (auto intersection = ray.template intersects<T>(triangle); intersection) {
                        results[thread_id].push_back(std::move(*intersection));
                    }
                }
                next = pop_node(thread_id);
            } else {
                next = std::nullopt;
                if (cur->has_left() && ray.intersects(cur->left().box())) {
                    next = &cur->left();
                }

                if (cur->has_right() && ray.intersects(cur->right().box())) {
                    if (next) queues[thread_id].push(&cur->right());
                    else next = &cur->right();
                }

                if (!next) {
                    next = pop_node(thread_id);
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

template<typename T>
std::vector<Vector3<typename T::float_t>> pool_intersects(
    const Ray<typename T::float_t>& ray,
    const KDTree<T>& tree,
    int threads_count
) {
    const auto& root = tree.top();
    if (!ray.intersects(root.box())) {
        return {};
    }

    ThreadPool<T> pool(ray, &root, threads_count);
    return pool.wait_result();
}

} // namespace rmi

#endif // RMI_INCLUDE_POOL


#ifdef RMI_INCLUDE_OMP
#    include "rmi.hpp"
#    include <vector>
#    include <omp.h>
namespace rmi::parallel {

template<typename T>
void omp_recursive_intersects(
    const Ray<typename T::float_t>& ray,
    const typename KDTree<T>::Node* node,
    std::vector<Vector3<typename T::float_t>>& output,
    double epsilon
) {
    if (node->is_leaf()) {
        for (const auto& cur : node->triangles()) {
            if (auto intersection = ray.template intersects<T>(cur, epsilon); intersection) {
                #pragma omp critical
                output.push_back(std::move(*intersection));
            }
        }
    } else {
        if (node->has_left() && ray.intersects(node->left().box())) {
            #pragma omp task shared(output)
            omp_recursive_intersects<T>(ray, &node->left(), output, epsilon);
        }

        if (node->has_right() && ray.intersects(node->right().box())) {
            #pragma omp task shared(output)
            omp_recursive_intersects<T>(ray, &node->right(), output, epsilon);
        }
    }
}


template<typename T>
std::vector<Vector3<typename T::float_t>> omp_intersects(
    const Ray<typename T::float_t>& ray,
    const KDTree<T>& tree,
    int threads_count,
    typename T::float_t epsilon
) {
    const auto& root = tree.top();
    if (!ray.intersects(root.box())) {
        return {};
    }

    std::vector<Vector3<typename T::float_t>> output;

    #pragma omp parallel shared(output) num_threads(threads_count)
    #pragma omp single
    omp_recursive_intersects<T>(ray, &root, output, epsilon);
    #pragma omp taskwait

    return output;
}


template<typename T, typename Splitter>
std::unique_ptr<typename KDTree<T>::Node> omp_recursive_build(
    const Range<typename T::iterator>& range,
    int depth,
    const Splitter& splitter
) {
    auto split = splitter(range, depth);
    if (split == range.end()) {
        return std::make_unique<typename KDTree<T>::Node>(get_bounding_box<T>(range), range);
    }
    std::unique_ptr<typename KDTree<T>::Node> left;
    std::unique_ptr<typename KDTree<T>::Node> right;

    #pragma omp task shared(left)
    left = std::move(omp_recursive_build<T>(Range(range.begin(), split), depth + 1, splitter));
    #pragma omp task shared(right)
    right = std::move(omp_recursive_build<T>(Range(split, range.end()), depth + 1, splitter));
    #pragma omp taskwait

    return std::make_unique<typename KDTree<T>::Node>(
        left->box() + right->box(),
        range,
        std::move(left), std::move(right)
    );
}


template<typename T, typename Splitter>
KDTree<T> omp_build(
    typename T::iterator begin,
    typename T::iterator end,
    int threads_count,
    const Splitter& splitter
) {
    std::unique_ptr<typename KDTree<T>::Node> node;
    #pragma omp parallel num_threads(threads_count) shared(node)
    #pragma omp single
    node = std::move(omp_recursive_build<T>(Range(begin, end), 0, splitter));

    return KDTree<T>(std::move(node));
}


template<typename mesh_t, typename float_t, typename index_t>
std::vector<Vector3<float_t>> omp_intersects(
    const Ray<float_t>& ray,
    Mesh<mesh_t, float_t, index_t>& mesh,
    int threads_count,
    float_t epsilon
) {
    std::vector<Vector3<float_t>> intersections;

    #pragma omp parallel for shared(mesh, ray, intersections) num_threads(threads_count)
    for (const auto& triangle : mesh) {
        if (auto intersection = ray.template intersects<mesh_t>(triangle, epsilon); intersection) {
            #pragma omp critical
            intersections.push_back(std::move(*intersection));
        }
    }

    return intersections;
}

} // namespace rmi

#endif // RMI_INCLUDE_OMP
