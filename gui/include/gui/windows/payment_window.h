#pragma once
#include "gui/base_window_t.h"
#include "logic/models/category_model.h"

class payment_window_t
    : public base_window_t
{
public:
    payment_window_t();
    ~payment_window_t() override = default;

    void update() override;

    std::string payment;
    category_model_t category;
    product_model_t product;

protected:
    bool m_inited = false;

};
