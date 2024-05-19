#pragma once

#include "kd_tree.hpp"
#include "ray.hpp"
#include "wsq.hpp"

#include <thread>


class ThreadPool {
public:
    ThreadPool(const Ray& ray, KDTree::Iterator root, int threads_count):
        threads(threads_count), queues(threads_count),  results(threads_count),
        counter(0), threads_count(threads_count), ray(ray)
    {
        distribute_load(root);

        for (int i = 0; i < threads_count; ++i) {
            threads[i] = std::thread(&ThreadPool::worker_thread, this, i);
        }
    }

    std::vector<Vector> wait_result() {
        std::vector<Vector> result;
        for (int id = 0; id < threads_count; ++id) {
            threads[id].join();
            std::copy(results[id].begin(), results[id].end(), std::back_inserter(result));
        }
        return result;
    }
private:
    void distribute_load(KDTree::Iterator root) {
        queues[0].push(std::move(root));
    }

    std::optional<KDTree::Iterator> pop_node(int thread_id) {
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
        std::optional<KDTree::Iterator> next = pop_node(thread_id);

        while(next) {
            KDTree::Iterator cur = next.value();

            if (cur.is_leaf()) {
                for (const Triangle& triangle : cur.triangles()) {
                    auto intersection = ray.intersects(triangle);
                    if (intersection.has_value()) {
                        results[thread_id].push_back(intersection.value());
                    }
                }
                next = pop_node(thread_id);
            } else {
                next = std::nullopt;

                if (auto left = cur.left(); ray.intersects(left.box())) {
                    next = left;
                }

                if (auto right = cur.right(); ray.intersects(right.box())) {
                    if (next) {
                        queues[thread_id].push(std::move(right));
                    } else {
                        next = right;
                    }
                } else if (!next) {
                    next = pop_node(thread_id);
                }
            }
        }
    }

    std::vector<std::thread> threads;
    std::vector<WorkStealingQueue<KDTree::Iterator>> queues;
    std::vector<std::vector<Vector>> results;

    std::atomic_int counter;
    int threads_count;

    const Ray& ray;
};
