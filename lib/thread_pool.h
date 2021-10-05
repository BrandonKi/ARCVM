/**
 * @file thread_pool.h
 * @author Brandon Kirincich
 */

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <concepts>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
  public:
    ThreadPool(size_t num_threads = std::thread::hardware_concurrency()):
        pool{}, task_queue{}, queue_mutex{}, manager{}, exit{false}
    {
        auto worker = [this] {
            while (true) {
                std::function<void(void)> task;
                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    this->manager.wait(lock, [this] { return this->exit || !this->task_queue.empty(); });
                    if (this->exit && this->task_queue.empty())
                        return;
                    task = std::move(this->task_queue.front());
                    this->task_queue.pop();
                }
                task();
            }
        };

        for (size_t i = 0; i < num_threads; ++i)
            pool.emplace_back(worker);
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            exit = true;
        }
        manager.notify_all();
        for (auto& thread : pool)
            thread.join();
    }

    template <std::invocable F, typename... Args>
    void push_work(F&& f, Args&&... args) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            task_queue.push([f]() { f(args...); });
        }
        manager.notify_one();
    }

    template <std::invocable F, typename... Args>
    [[nodiscard]] auto push_task(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
        using return_type = std::invoke_result_t<F, Args...>;
        auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        std::future<return_type> result = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            task_queue.push([task]() { (*task)(); });
        }
        manager.notify_one();
        return result;
    }

  private:
    std::vector<std::thread> pool;
    std::queue<std::function<void(void)>> task_queue;

    std::mutex queue_mutex;
    std::condition_variable manager;
    bool exit;
};

#endif // THREAD_POOL_H