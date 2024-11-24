#pragma once
#include "gui/base_window_t.h"


class product_value_window_t
    : public base_window_t
{
public:
    product_value_window_t();
    ~product_value_window_t() override = default;

    void update() override;

    std::string product_value;

protected:
    bool m_inited = false;
    inline static size_t m_count = 0;

};
