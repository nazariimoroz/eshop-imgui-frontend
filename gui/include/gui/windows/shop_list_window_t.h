#pragma once
#include <optional>

#include "gui/base_window_t.h"
#include "gui/gui.h"
#include "logic/fabrics/product_fabric.h"

class payment_window_t;

class shop_list_window_t
    : public base_window_t
{
public:
    shop_list_window_t();
    ~shop_list_window_t() override;

    void update() override;

    void try_get_all_products();

    void open_payment_window(const char* payment,
                             const category_model_t& category,
                             const product_model_t& product);

protected:
    bool m_inited = false;
    std::vector<category_model_t> m_categories;
    std::weak_ptr<payment_window_t> m_payment_window;

protected:
    void categories_loaded_callback(const std::optional<std::vector<category_model_t>>& models,
                                    const std::string& message);

    void paymentw_on_begin_destroy_callback();

};
