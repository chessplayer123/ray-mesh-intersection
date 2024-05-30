#pragma once

#include <thread>

#include "tree.hpp"
#include "wsq.hpp"
#include "ray.hpp"


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
        threads(threads_count), queues(threads_count),  results(threads_count),
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
