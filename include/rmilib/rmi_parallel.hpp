#pragma once


#ifdef RMI_INCLUDE_POOL
#    include "rmi.hpp"
#    include "wsq.hpp"
#    include <thread>
namespace rmi::parallel {

template<typename T, int N>
std::vector<Vector3<typename T::float_t>> pool_intersects(
    const Ray<typename T::float_t>& ray,
    const Tree<T, N>& tree,
    int threads_count
);

}
#endif // RMI_INCLUDE_POOL


#ifdef RMI_INCLUDE_OMP
#    include "rmi.hpp"
#    include <vector>
#    include <omp.h>
namespace rmi::parallel {

template<typename T, int N>
std::vector<Vector3<typename T::float_t>> omp_intersects(
    const Ray<typename T::float_t>& ray,
    const Tree<T, N>& tree,
    int threads_count,
    double epsilon = std::numeric_limits<double>::epsilon()
);


template<typename T, int N, typename Splitter = rmi::SAHSplitter<T, N>>
Tree<T, N> omp_build(
    typename T::iterator begin,
    typename T::iterator end,
    int threads_count,
    const Splitter& splitter = Splitter()
);

}
#endif // RMI_INCLUDE_OMP



#ifdef RMI_INCLUDE_POOL
namespace rmi::parallel {

template<typename T, int N>
class ThreadPool {
public:
    typedef typename Tree<T, N>::Node Node;
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
            const Node* cur = next.value();

            if (cur->is_leaf()) {
                for (const auto& triangle : cur->triangles()) {
                    auto intersection = ray.template intersects<T>(triangle);
                    if (intersection.has_value()) {
                        results[thread_id].push_back(intersection.value());
                    }
                }
                next = pop_node(thread_id);
            } else {
                next = std::nullopt;

                for (const auto& child : cur->child_nodes()) {
                    if (ray.intersects(child.box())) {
                        if (next) queues[thread_id].push(&child);
                        else next = &child;
                    }
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

template<typename T, int N>
std::vector<Vector3<typename T::float_t>> pool_intersects(
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

} // namespace rmi

#endif // RMI_INCLUDE_POOL


#ifdef RMI_INCLUDE_OMP
#    include "rmi.hpp"
#    include <vector>
#    include <omp.h>
namespace rmi::parallel {

template<typename T, int N>
void omp_recursive_intersects(
    const Ray<typename T::float_t>& ray,
    const typename Tree<T, N>::Node* node,
    std::vector<Vector3<typename T::float_t>>& output,
    double epsilon
) {
    if (node->is_leaf()) {
        for (const auto& cur : node->triangles()) {
            auto intersection = ray.template intersects<T>(cur, epsilon);
            if (intersection.has_value()) {
                #pragma omp critical
                output.push_back(intersection.value());
            }
        }
    } else {
        for (const auto& child : node->child_nodes()) {
            if (ray.intersects(child.box())) {
                const auto* child_ptr = &child;
                #pragma omp task shared(output)
                omp_recursive_intersects<T, N>(ray, child_ptr, output, epsilon);
            }
        }
    }
}


template<typename T, int N>
std::vector<Vector3<typename T::float_t>> omp_intersects(
    const Ray<typename T::float_t>& ray,
    const Tree<T, N>& tree,
    int threads_count,
    double epsilon
) {
    const auto& root = tree.top();
    if (!ray.intersects(root.box())) {
        return {};
    }

    std::vector<Vector3<typename T::float_t>> output;

    #pragma omp parallel shared(output) num_threads(threads_count)
    #pragma omp single
    omp_recursive_intersects<T, N>(ray, &root, output, epsilon);
    #pragma omp taskwait

    return output;
}


template<typename T, int N, typename Splitter>
typename Tree<T, N>::Node omp_recursive_build(
    const Range<typename T::iterator>& range,
    const Splitter& splitter
) {
    auto subranges = splitter(range);
    std::vector<typename Tree<T, N>::Node> children;
    AABBox<typename T::float_t> aabb;

    #pragma omp taskloop shared(children, aabb, subranges)
    for (const auto& subrange : subranges) {
        auto child = subrange.is_splittable() ?
            omp_recursive_build<T, N>(subrange, splitter) :
            typename Tree<T, N>::Node(get_bounding_box<T>(subrange), subrange);
        #pragma omp critical
        {
            aabb += child.box();
            children.push_back(std::move(child));
        }
    }

    return typename Tree<T, N>::Node(aabb, range, std::move(children));
}


template<typename T, int N, typename Splitter>
Tree<T, N> omp_build(
    typename T::iterator begin,
    typename T::iterator end,
    int threads_count,
    const Splitter& splitter
) {
    typename Tree<T, N>::Node node(
        AABBox<typename T::float_t>(),
        Range(end, end)
    );
    #pragma omp parallel num_threads(threads_count)
    #pragma omp single
    node = std::move(omp_recursive_build<T, N>(Range(begin, end), splitter));

    return Tree<T, N>(std::move(node));
}

} // namespace rmi

#endif // RMI_INCLUDE_OMP
