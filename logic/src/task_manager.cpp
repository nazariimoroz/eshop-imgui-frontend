#include "logic/utilities/task_manager.h"

#include <iostream>
#include <memory>
#include <stdexcept>

task_manager_t::task_manager_t() = default;

task_manager_t::~task_manager_t() = default;

task_manager_t* task_manager_t::create()
{
    m_singleton = std::unique_ptr<task_manager_t>(new task_manager_t);
    return m_singleton.get();
}

task_manager_t& task_manager_t::get()
{
#ifndef NDEBUG
    if (!m_singleton)
    {
        std::cerr << "Cant get task manager" << std::endl;
        exit(-1);
    }
#endif

    return *m_singleton;
}

void task_manager_t::add_async_callback(callback_t&& callback)
{
    m_thread_pool.detach_task([callback = std::move(callback)]{ callback(); });
}

void task_manager_t::add_callback(callback_t&& callback)
{
    std::unique_lock mtx{m_callbacks_mutex};

    m_callbacks.push(std::move(callback));
}

void task_manager_t::process()
{
    std::unique_lock mtx{m_callbacks_mutex};

    if (m_callbacks.empty())
        return;

    m_callbacks.back()();

    m_callbacks.pop();
}

void task_manager_t::wait()
{
    //thread_pool.wait();
}
