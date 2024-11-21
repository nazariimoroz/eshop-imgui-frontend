#pragma once

#include <rfl.hpp>

struct product_model_t
{
    std::string name;
    std::string description;
    rfl::Description<"Prise in usd", uint32_t> price;
    rfl::Description<"Amount, -1 means infinity", uint32_t> amount;
    rfl::Rename<"category", uint32_t> category_id;
};
