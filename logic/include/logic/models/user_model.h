#pragma once

#include <string>
#include <rfl.hpp>

enum class user_type_t
{
    customer, admin
};

class user_model_t
{
public:
    rfl::Email email;
    std::string crypted_password;

    user_type_t user_type;

    // will be inited only on local user
    std::optional<std::string> jwt;

    rfl::Timestamp<"%Y-%m-%d-%h-%m-%s"> creation_date;
};
