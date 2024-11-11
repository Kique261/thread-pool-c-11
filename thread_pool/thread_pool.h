#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "heads.h"
#include <functional> // 包含 std::bind 和 std::invoke_result

class thread_pool {
private:
    std::once_flag init_flag; // 用于保护初始化
    size_t threads_num;
    std::mutex queue_mutex; // 用于操作任务队列
    std::queue<std::function<void()>> tasks;
    std::vector<std::thread> workers; // 使用 std::thread 而不是 std::jthread
    std::atomic<bool> stop; // 用于析构
    void init_pool(); // 真正的初始化
public:
    thread_pool();
    ~thread_pool();
    size_t get_threads_num(){return threads_num;};
    template<class Callable, class... Args>
    auto task_in(Callable&& function, Args&&... args) -> std::future<std::invoke_result_t<Callable, Args...>>;
};

template<class Callable, class... Args>
auto thread_pool::task_in(Callable&& function, Args&&... args) -> std::future<std::invoke_result_t<Callable, Args...>> {
    using return_type = std::invoke_result_t<Callable, Args...>;
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<Callable>(function), std::forward<Args>(args)...)
    );
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(this->queue_mutex);
        if (this->stop.load()) {
            throw std::runtime_error("try to push task into a stopped pool!");
        }
        this->tasks.emplace([task]() { (*task)(); });
    }
    return res;
}

#endif