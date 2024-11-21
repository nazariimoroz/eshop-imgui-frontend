#pragma once
#include <functional>
#include <memory>
#include <string>
#include <rfl/patterns.hpp>

class user_model_t;

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

protected:
    void send_error_callback(const std::string& message) const;

};
