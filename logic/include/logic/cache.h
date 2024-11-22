#pragma once

#include <memory>
#include <string>

#include "models/user_model.h"

class cache_t
{
    cache_t();

public:
    ~cache_t();
    cache_t(const cache_t&) = delete;
    cache_t& operator=(const cache_t&) = delete;
    cache_t(cache_t&&) = delete;
    cache_t& operator=(cache_t&&) = delete;

    static cache_t* create();

    static cache_t& get();

    std::weak_ptr<user_model_t> get_user_model() const { return user_model; }
    void set_user_model(const std::shared_ptr<user_model_t>& in_user_model);

    [[nodiscard]] std::string get_jwt() const;
    void set_jwt(const std::string& in_jwt);

protected:
    std::shared_ptr<user_model_t> user_model = nullptr;
    std::string jwt;

private:
    inline static std::unique_ptr<cache_t> m_singleton = nullptr;
};
