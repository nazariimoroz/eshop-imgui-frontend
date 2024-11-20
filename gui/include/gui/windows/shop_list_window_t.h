#pragma once
#include "gui/base_window_t.h"

class shop_list_window_t
    : public base_window_t
{
public:
    shop_list_window_t();
    ~shop_list_window_t() override = default;

    void update() override;
};
