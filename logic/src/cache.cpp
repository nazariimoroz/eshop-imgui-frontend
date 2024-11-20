#include "logic/cache.h"

#include <iostream>
#include <rfl.hpp>
#include <rfl/json.hpp>

#include "logic/models/user_model.h"

cache_t::cache_t()
{
    std::ifstream ifs{};

    ifs.open("cache_user.json");
    if (ifs.is_open())
    {
        std::string str((std::istreambuf_iterator<char>(ifs)),
                        std::istreambuf_iterator<char>());
        user_model = std::make_shared<user_model_t>(
            rfl::json::read<user_model_t>(str).value()
        );

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
