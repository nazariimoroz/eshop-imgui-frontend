#pragma once
#include <optional>

#include "gui/base_window_t.h"
#include "logic/fabrics/product_fabric.h"

class shop_list_window_t
    : public base_window_t
{
public:
    shop_list_window_t();
    ~shop_list_window_t() override = default;

    void update() override;

    void try_get_all_products();

protected:
    bool m_inited = false;
    std::vector<category_model_t> m_categories;

protected:
    void loaded_callback(const std::optional<product_fabric_t::ctg_models_vec_t>& models,
        const std::string& message);

};
