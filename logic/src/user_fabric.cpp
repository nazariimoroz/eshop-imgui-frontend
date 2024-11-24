#include "logic/fabrics/user_fabric.h"

#include <stdexcept>

#include "../../.env.h"
#include "logic/cache.h"

#include <rfl.hpp>
#include <rfl/json.hpp>

#include "httplib/httplib.h"
#include "logic/utilities/task_manager.h"

struct api_create_user_body_t
{
    std::string email;
    std::string password;
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

    auto cli = std::make_shared<httplib::Client>(BACKEND_HOST);
    if (!cli->is_valid())
    {
        return {false, "Cant open server"};
    }

    auto self = shared_from_this();
    task_manager_t::get().add_async_callback([self, cli, email, password]()
    {
        api_create_user_body_t body{email, password};

        const auto result = cli->Post("/api/create_user",
                                      rfl::json::write(body),
                                      "application/json");

        if (result.error() != httplib::Error::Success || result.value().status != 200)
        {
            const auto message = result
                                     ? rfl::json::read<api_error_response_t>(result->body)
                                       .value_or({"unknown error"}).message
                                     : "unknown error";

            self->send_error_callback(message);

            return;
        }

        const auto response = rfl::json::read<api_user_ok_response_t>(result.value().body).value();

        self->send_ok_callback(response);
    });

    return {true, "ok"};
}

std::tuple<bool, std::string> user_fabric_t::load(const std::string& email, const std::string& password) const
{
    auto [ok, error] = validate(email, password);
    if (!ok)
        return {ok, error};

    auto cli = std::make_shared<httplib::Client>(BACKEND_HOST);
    if (!cli->is_valid())
    {
        return {false, "Cant open server"};
    }

    auto self = shared_from_this();
    task_manager_t::get().add_async_callback([self, cli, email, password]()
    {
        api_get_user_body_t body{"", email, password};

        std::string a = rfl::json::write(body);
        const auto result = cli->Post("/api/get_user",
                                      rfl::json::write(body),
                                      "application/json");

        if (result.error() != httplib::Error::Success || result.value().status != 200)
        {
            const auto message = result
                                     ? rfl::json::read<api_error_response_t>(result->body)
                                       .value_or({"unknown error"}).message
                                     : "unknown error";

            self->send_error_callback(message);

            return;
        }

        const auto response = rfl::json::read<api_user_ok_response_t>(result.value().body).value();

        self->send_ok_callback(response);
    });

    return {true, "ok"};
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

    auto self = shared_from_this();
    task_manager_t::get().add_async_callback([self, cli, jwt]()
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

            self->send_error_callback(message);

            return;
        }

        const auto response = rfl::json::read<api_user_ok_response_t>(result.value().body).value();

        self->send_ok_callback(response);
    });

    return {true, "ok"};
}

void user_fabric_t::send_ok_callback(const api_user_ok_response_t& response) const
{
    task_manager_t::get().add_callback([self = shared_from_this(), response]()
    {
        auto user = std::make_shared<user_model_t>(response.user);
        cache_t::get().set_user_model(user);

        cache_t::get().set_jwt(response.jwt);

        if (self->context_window == std::nullopt)
        {
            std::cerr << "user_fabric: context window dont set" << std::endl;
        }
        else if (const auto w = self->context_window->lock())
        {
            self->loaded_callback(true, "ok");
        }
    });
}

void user_fabric_t::send_error_callback(const std::string& message) const
{
    if (loaded_callback)
    {
        task_manager_t::get().add_callback([self = shared_from_this(), message]()
        {
            if (self->context_window == std::nullopt)
            {
                std::cerr << "user_fabric: context window dont set" << std::endl;
            }
            else if (const auto w = self->context_window->lock())
            {
                self->loaded_callback(false, message);
            }
        });
    }
}
