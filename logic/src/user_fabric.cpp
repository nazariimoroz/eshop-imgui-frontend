#include "logic/user_fabric.h"

#include <stdexcept>

#include "../../.env.h"
#include "logic/cache.h"
#include "logic/models/user_model.h"

#include <rfl.hpp>
#include <rfl/json.hpp>

#include "httplib/httplib.h"
#include "logic/utilities/task_manager.h"

struct api_create_user_body_t
{
    std::string email;
    std::string password;
};

struct api_create_user_response_t
{
    user_model_t user;
    std::string jwt;
};

struct api_error_response_t
{
    std::string message;
};

struct api_get_user_body_t
{
    std::string jwt;
    std::string email;
    std::string password;
};

struct api_get_user_response_t
{
    user_model_t user;
    std::string jwt;
};

user_fabric_t::user_fabric_t(private_t)
{
}

user_fabric_t::~user_fabric_t() = default;

std::tuple<bool, std::string> user_fabric_t::validate(const std::string& email,
                                                      const std::string& password) const
{
    if (email.empty() || password.empty())
        return {false, "Empty email or password"};

    std::regex email_regex(R"(^[\w-\.]+@([\w-]+\.)+[\w-]{2,4}$)");
    if (!std::regex_match(email.begin(), email.end(), email_regex)
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
    if (email.empty() || password.empty())
        return {false, "Empty email or password"};

    if (password != password_repeat)
        return {false, "Password not equal password repeat"};

    std::regex email_regex(R"(^[\w-\.]+@([\w-]+\.)+[\w-]{2,4}$)");
    if (!std::regex_match(email.begin(), email.end(), email_regex)
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
        return {ok, error};

    httplib::Client cli(BACKEND_HOST);

    api_create_user_body_t body{email, password};

    const auto result = cli.Post("/api/create_user",
                                 rfl::json::write(body),
                                 "application/json");

    if (result.error() != httplib::Error::Success || result.value().status != 200)
    {
        const auto message = result
                                 ? rfl::json::read<api_error_response_t>(result->body)
                                   .value_or({"unknown error"}).message
                                 : "unknown error";
        return {false, message};
    }

    const auto response = rfl::json::read<api_create_user_response_t>(result.value().body).value();

    auto user = std::make_shared<user_model_t>(response.user);
    cache_t::get().set_user_model(user);

    cache_t::get().set_jwt(response.jwt);

    return {true, "ok"};
}

std::tuple<bool, std::string> user_fabric_t::load(const std::string& email, const std::string& password) const
{
    auto [ok, error] = validate(email, password);
    if (!ok)
        return {ok, error};

    httplib::Client cli(BACKEND_HOST);

    api_get_user_body_t body{"", email, password};

    std::string a = rfl::json::write(body);
    const auto result = cli.Post("/api/get_user",
                                 rfl::json::write(body),
                                 "application/json");

    if (result.error() != httplib::Error::Success || result.value().status != 200)
    {
        const auto message = result
                                 ? rfl::json::read<api_error_response_t>(result->body)
                                   .value_or({"unknown error"}).message
                                 : "unknown error";
        return {false, message};
    }

    const auto response = rfl::json::read<api_get_user_response_t>(result.value().body).value();

    auto user = std::make_shared<user_model_t>(response.user);
    cache_t::get().set_user_model(user);

    cache_t::get().set_jwt(response.jwt);

    return {true, "TODO"};
}

std::tuple<bool, std::string> user_fabric_t::load_jwt(const std::string& jwt) const
{
    if (jwt.empty())
        return {false, "no jwt"};

    auto cli = std::make_shared<httplib::Client>(BACKEND_HOST);
    if (!cli->is_valid())
    {
        return {false, "Cant open server"};
    }

    task_manager_t::get().add_async_callback([cli, jwt]()
    {
        api_get_user_body_t body{jwt, "", ""};

        const auto result = cli->Post("/api/get_user",
                                      rfl::json::write(body),
                                      "application/json");

        if (result.error() != httplib::Error::Success || result.value().status != 200)
        {
            cache_t::get().set_jwt("");

            const auto message = result
                                     ? rfl::json::read<api_error_response_t>(result->body)
                                       .value_or({"unknown error"}).message
                                     : "unknown error";
            std::cerr << "Error: " << message << std::endl;
        }

        const auto response = rfl::json::read<api_get_user_response_t>(result.value().body).value();

        task_manager_t::get().add_callback([response]()
        {
            auto user = std::make_shared<user_model_t>(response.user);
            cache_t::get().set_user_model(user);

            cache_t::get().set_jwt(response.jwt);
        });
    });

    return {true, "ok"};
}
