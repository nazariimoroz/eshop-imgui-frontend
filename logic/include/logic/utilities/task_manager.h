#pragma once

#include <BS_thread_pool.hpp>
#include <functional>
#include <memory>
#include <mutex>

class task_manager_t
{
private:
    task_manager_t();

public:
    using callback_t = std::function<void()>;

public:
    ~task_manager_t();
    task_manager_t(const task_manager_t&) = delete;
    task_manager_t& operator=(const task_manager_t&) = delete;
    task_manager_t(task_manager_t&&) = delete;
    task_manager_t& operator=(task_manager_t&&) = delete;

    static task_manager_t* create();

    static task_manager_t& get();

    void add_async_callback(callback_t&& callback);
    void add_callback(callback_t&& callback);

    void process();
    void wait();

protected:
    BS::thread_pool m_thread_pool;
    std::queue<callback_t> m_callbacks;

protected:
    mutable std::mutex m_callbacks_mutex;

private:
    inline static std::unique_ptr<task_manager_t> m_singleton = nullptr;
};
