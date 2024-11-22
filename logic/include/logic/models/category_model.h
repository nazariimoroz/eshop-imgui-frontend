#pragma once

#include <rfl.hpp>

#include "product_model.h"

struct category_model_t
{
    int32_t id;
    std::string name;
    std::vector<product_model_t> products;
};
