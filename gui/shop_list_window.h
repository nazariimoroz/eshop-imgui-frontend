#pragma once
#include "base_window.h"

class shop_list_window
    : public base_window
{
public:
    shop_list_window();
    ~shop_list_window() override = default;

    void update() override;
};
