#pragma once

#include <string>
#include <rfl.hpp>

class user_model_t
{
public:
    rfl::Email email;
    std::string crypted_password;

    rfl::Timestamp<"%Y-%m-%d-%h-%m-%s"> creation_date;
};
