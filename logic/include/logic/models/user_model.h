#pragma once

#include <string>
#include <rfl.hpp>
#include <rfl/json.hpp>

enum class user_type_t
{
    customer, admin, unknown
};

struct user_model_t
{
    rfl::Email email;

    /** Can be encrypted in some context */
    std::string crypted_password;

    user_type_t user_type;

    /** will be inited only on local user */
    std::optional<std::string> jwt;

    std::optional<uint64_t> creation_date;
};
