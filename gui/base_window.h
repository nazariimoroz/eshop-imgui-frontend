#pragma once
#include <string>

class base_window
{
public:
    base_window() = default;
    virtual ~base_window() = default;
    base_window(const base_window&) = delete;
    base_window& operator=(const base_window&) = delete;
    base_window(base_window&&) = default;
    base_window& operator=(base_window&&) = default;

    virtual void update() = 0;

    [[nodiscard]] bool is_shown() const { return m_show; }
    void set_show(const bool show) { m_show = show; }

    [[nodiscard]] const std::string& get_name() const { return name; };

protected:
    bool m_show = false;
    std::string name;

};

