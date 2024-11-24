#include "logic/payment/apple_payment.h"

#include "../../.env.h"

#include <httplib.h>
#include <stdexcept>
#include <rfl/json/read.hpp>

#include "logic/models/category_model.h"
#include "logic/utilities/task_manager.h"

#include "uuid.h"
#include "logic/cache.h"

struct ap_api_error_response_t
{
    std::string message;
};

struct ap_google_identify_response_t
{
    int payment_id;
    std::string link;
};

struct ap_new_apple_payment_body_t
{
    std::string jwt;
    int product_id;
};

struct ap_check_payment_body_t
{
    std::string jwt;
    int payment_id;
};

struct ap_check_payment_response_t
{
    std::string status;
    std::optional<std::string> product_value;
};

apple_payment_t::apple_payment_t(protected_t p) : base_payment_t(p)
{
}

const char* apple_payment_t::name()
{
    return "Apple Pay";
}

std::tuple<bool, std::string> apple_payment_t::process(
    const category_model_t& category,
    const product_model_t& product)
{
    auto cli = std::make_shared<httplib::Client>(BACKEND_HOST);
    if (!cli->is_valid())
    {
        return {false, "Cant open server"};
    }

    auto jwt = cache_t::get().get_jwt();

    auto self = apple_shared_from_this();
    task_manager_t::get().add_async_callback([self, category, product, cli, jwt]()
    {
        self->nt_create_payment(cli, category, product, jwt);
    });

    return {true, "ok"};
}

void apple_payment_t::nt_create_payment(
    const std::shared_ptr<httplib::Client>& cli,
    const category_model_t& category,
    const product_model_t& local_product,
    const std::string& jwt)
{
    auto product = nt_refresh_product_info(cli, category, local_product);
    if (!product)
        return;

    ap_new_apple_payment_body_t body{
        .jwt = std::move(jwt),
        .product_id = product->id
    };

    const auto result = cli->Post("/api/new_apple_payment/",
                                  rfl::json::write(body),
                                  "application/json");
    if (!nt_check_req_result_for_create_payment(result, "unknown error")) return;

    const auto pay_ind = rfl::json::read<payment_identificator_t>(result.value().body).value();

    mt_send_payment_created_callback(std::move(pay_ind), "ok");
}

std::optional<product_model_t> apple_payment_t::nt_refresh_product_info(
    const std::shared_ptr<httplib::Client>& cli,
    const category_model_t& category,
    const product_model_t& product)
{
    httplib::Params params{
        {"category_id", std::to_string(category.id)},
        {"product_id", std::to_string(product.id)}
    };
    const auto result = cli->Get("/api/get_product/", params, httplib::Headers{});
    if (!nt_check_req_result_for_create_payment(result, "unknown error")) return std::nullopt;

    const auto response = rfl::json::read<product_model_t>(result.value().body).value();
    return response;
}

bool apple_payment_t::nt_check_req_result_for_create_payment(const httplib::Result& result, const std::string& other_message)
{
    if (result.error() != httplib::Error::Success || result.value().status != 200)
    {
        const auto message = result
                                 ? rfl::json::read<ap_api_error_response_t>(result->body)
                                   .value_or({other_message}).message
                                 : other_message;

        auto self = apple_shared_from_this();
        task_manager_t::get().add_callback([self, message]()
        {
            self->mt_send_payment_created_callback(std::nullopt, message);
        });

        return false;
    }
    return true;
}

void apple_payment_t::mt_send_payment_created_callback(std::optional<payment_identificator_t>&& payment_identificator,
                                                        const std::string& message)
{
    if (context_window == std::nullopt)
    {
        std::cerr << "google_payment: context window dont set" << std::endl;
    }
    else if (const auto w = context_window->lock())
    {
        if (payment_created_callback)
        {
            payment_created_callback(payment_identificator, message);
        }
    }
}

std::tuple<bool, std::string> apple_payment_t::check_payment(const payment_identificator_t& payment_identificator)
{
    auto cli = std::make_shared<httplib::Client>(BACKEND_HOST);
    if (!cli->is_valid())
    {
        return {false, "Cant open server"};
    }

    auto jwt = cache_t::get().get_jwt();

    auto self = apple_shared_from_this();
    task_manager_t::get().add_async_callback([self, payment_identificator, jwt, cli]()
    {
        self->nt_check_payment(payment_identificator, jwt, cli);
    });

    return {true, "ok"};
}

void apple_payment_t::nt_check_payment(const payment_identificator_t& payment_identificator,
                                        const std::string& jwt,
                                        const std::shared_ptr<httplib::Client>& cli)
{
    ap_check_payment_body_t body{
        .jwt = std::move(jwt),
        .payment_id = payment_identificator.payment_id
    };

    const auto result = cli->Post("/api/check_payment/",
                                  rfl::json::write(body),
                                  "application/json");
    if (!nt_check_req_result_for_check_payment(result, "unknown error")) return;
    const auto response = rfl::json::read<ap_check_payment_response_t>(result.value().body).value();

    auto self = apple_shared_from_this();
    task_manager_t::get().add_callback([self, response]()
    {
        if (response.status != "paid")
        {
            self->mt_send_payment_done_callback(std::nullopt,
                                                std::format("Product status is: {}", response.status));
        }
        else
        {
            self->mt_send_payment_done_callback(response.product_value,
                                                "ok");
        }
    });
}

bool apple_payment_t::nt_check_req_result_for_check_payment(const httplib::Result& result,
    const std::string& other_message)
{
    if (result.error() != httplib::Error::Success || result.value().status != 200)
    {
        const auto message = result
                                 ? rfl::json::read<ap_api_error_response_t>(result->body)
                                   .value_or({other_message}).message
                                 : other_message;

        auto self = apple_shared_from_this();
        task_manager_t::get().add_callback([self, message]()
        {
            self->mt_send_payment_done_callback(std::nullopt, message);
        });

        return false;
    }
    return true;
}

void apple_payment_t::mt_send_payment_done_callback(
    std::optional<std::string> product_value,
    const std::string& message)
{
    if (context_window == std::nullopt)
    {
        std::cerr << "google_payment: context window dont set" << std::endl;
    }
    else if (const auto w = context_window->lock())
    {
        if (payment_done_callback)
        {
            payment_done_callback(product_value, message);
        }
    }
}

std::shared_ptr<apple_payment_t> apple_payment_t::apple_shared_from_this()
{
    return std::dynamic_pointer_cast<apple_payment_t>(shared_from_this());
}
