#pragma once
#include <functional>
#include <memory>
#include <string>

#include "logic/models/category_model.h"

class product_fabric_t
    : public std::enable_shared_from_this<product_fabric_t>
{
    struct private_t{ explicit private_t() = default; };

public:
    using ctg_models_vec_t = std::vector<category_model_t>;

public:
    product_fabric_t(private_t);
    ~product_fabric_t();

    static std::shared_ptr<product_fabric_t> create()
    {
        return std::make_shared<product_fabric_t>(private_t{});
    }

    /** [ok, error_reason], result in cache */
    std::tuple<bool, std::string> load_all() const;

    std::function<void(const std::optional<ctg_models_vec_t>& models, std::string message)> loaded_callback;

protected:
    void send_callback(const std::optional<ctg_models_vec_t>& models,
        const std::string& message) const;

};
