#include "logic/user_fabric.h"

#include <stdexcept>

#include "logic/cache.h"
#include "logic/models/user_model.h"

user_fabric_t::user_fabric_t(private_t)
{

}

user_fabric_t::~user_fabric_t() = default;

std::tuple<bool, std::string> user_fabric_t::validate(const std::string& email,
    const std::string& password) const
{
    if(email.empty() || password.empty())
        return {false, "Empty email or password"};

    std::regex email_regex(R"(^[\w-\.]+@([\w-]+\.)+[\w-]{2,4}$)");
    if(!std::regex_match(email.begin(), email.end(), email_regex)
        || password.size() < 8)
    {
        return {false, "Bad email or password"};
    }

    return {true, "ok"};
}

std::tuple<bool, std::string> user_fabric_t::validate(const std::string& email,
    const std::string& password,
    const std::string& password_repeat) const
{
    if(email.empty() || password.empty())
        return {false, "Empty email or password"};

    if(password != password_repeat)
        return {false, "Password not equal password repeat"};

    std::regex email_regex(R"(^[\w-\.]+@([\w-]+\.)+[\w-]{2,4}$)");
    if(!std::regex_match(email.begin(), email.end(), email_regex)
        || password.size() < 8)
    {
        return {false, "Bad email or password"};
    }

    return {true, "ok"};
}

std::tuple<bool, std::string> user_fabric_t::save(const std::string& email,
    const std::string& password,
    const std::string& password_repeat) const
{
    auto [ok, error] = validate(email, password, password_repeat);
    if (!ok)
        return { ok, error};

    /*
     * SERVER ASYNC CODE
     */

    auto user = std::make_shared<user_model_t>(
        email,
        password,
        user_type_t::customer,
        std::nullopt,
        std::chrono::system_clock::now().time_since_epoch().count()
    );
    cache_t::get().set_user_model(user);

    return {true, "ok"};
}

std::tuple<bool, std::string> user_fabric_t::load(const std::string& email, const std::string& password) const
{
    return {false, "TODO"};
}

