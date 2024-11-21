#include "logic/cache.h"

#include <iostream>
#include <rfl.hpp>
#include <rfl/json.hpp>

#include "logic/models/user_model.h"

struct cache_struct_t
{
    std::string jwt;
};

cache_t::cache_t()
{
    std::ifstream ifs{};

    ifs.open("cache.json");
    if (ifs.is_open())
    {
        std::string str((std::istreambuf_iterator<char>(ifs)),
                        std::istreambuf_iterator<char>());
        const auto result = rfl::json::read<cache_struct_t>(str).value();

        jwt = result.jwt;

        ifs.close();
    }
}

cache_t::~cache_t() = default;

cache_t* cache_t::create()
{
    m_singleton = std::unique_ptr<cache_t>(new cache_t);
    return m_singleton.get();
}

cache_t& cache_t::get()
{
#ifndef NDEBUG
    if (!m_singleton)
    {
        std::cerr << "Cant get gui" << std::endl;
        exit(-1);
    }
#endif

    return *m_singleton;
}

void cache_t::set_user_model(const std::shared_ptr<user_model_t>& in_user_model)
{
    user_model = in_user_model;

    std::string result = rfl::json::write(*user_model);

    /*
    std::ofstream ifs{};

    ifs.open("cache_user.json");
    if (ifs.is_open())
    {
        ifs << result;
        ifs.close();
    }*/
}

std::string cache_t::get_jwt() const
{
    return jwt;
}

void cache_t::set_jwt(const std::string& in_jwt)
{
    jwt = in_jwt;

    std::ofstream ifs{};
    ifs.open("cache.json");
    if (ifs.is_open())
    {
        ifs << rfl::json::write(cache_struct_t{
            .jwt = jwt
        });
        ifs.close();
    }
}
