#pragma once
#include <functional>
#include <string>

class base_window_t
{
public:
    base_window_t() = default;
    virtual ~base_window_t();
    base_window_t(const base_window_t&) = delete;
    base_window_t& operator=(const base_window_t&) = delete;
    base_window_t(base_window_t&&) = default;
    base_window_t& operator=(base_window_t&&) = default;

    virtual void update() = 0;

    [[nodiscard]] bool is_shown() const { return m_show; }
    void set_show(const bool show) { m_show = show; }

    [[nodiscard]] const std::string& get_name() const { return name; };

    std::function<void()> on_begin_destroy_callback;

protected:
    bool m_show = false;
    std::string name;

};

