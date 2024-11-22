#pragma once
#include <functional>
#include <memory>
#include <string>
#include <rfl/patterns.hpp>

#include "logic/models/user_model.h"

class base_window_t;

struct api_user_ok_response_t
{
    user_model_t user;
    std::string jwt;
};

class user_fabric_t
    : public std::enable_shared_from_this<user_fabric_t>
{
    struct private_t{ explicit private_t() = default; };

public:
    user_fabric_t(private_t);
    ~user_fabric_t();

    static std::shared_ptr<user_fabric_t> create()
    {
        return std::make_shared<user_fabric_t>(private_t{});
    }

    /** [ok, error_reason] */
    std::tuple<bool, std::string> validate(
        const std::string& email,
        const std::string& password) const;

    /** [ok, error_reason] */
    std::tuple<bool, std::string> validate(
        const std::string& email,
        const std::string& password,
        const std::string& password_repeat) const;

    /** [ok, error_reason], result in cache */
    std::tuple<bool, std::string> save(
        const std::string& email,
        const std::string& password,
        const std::string& password_repeat) const;

    /** [ok, error_reason], result in cache */
    std::tuple<bool, std::string> load(
        const std::string& email,
        const std::string& password) const;

    /** [ok, error_reason], result in cache */
    std::tuple<bool, std::string> load_jwt(
        const std::string& jwt) const;

    std::function<void(bool error, std::string message)> loaded_callback;

    std::optional<std::weak_ptr<base_window_t>> context_window;

protected:
    void send_ok_callback(const api_user_ok_response_t& response) const;

    void send_error_callback(const std::string& message) const;

};
