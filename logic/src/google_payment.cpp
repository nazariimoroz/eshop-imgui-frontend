#include "logic/payment/google_payment.h"

#include "../../.env.h"

#include <httplib.h>
#include <stdexcept>
#include <rfl/json/read.hpp>

#include "logic/models/category_model.h"
#include "logic/utilities/task_manager.h"

#include "uuid.h"

struct gp_api_error_response_t
{
    std::string message;
};

google_payment_t::google_payment_t(protected_t p) : base_payment_t(p)
{
}

const char* google_payment_t::name()
{
    return "Google Pay";
}

std::tuple<bool, std::string> google_payment_t::process(
    const category_model_t& category,
    const product_model_t& product)
{
    auto cli = std::make_shared<httplib::Client>(BACKEND_HOST);
    if (!cli->is_valid())
    {
        return {false, "Cant open server"};
    }

    auto self = google_shared_from_this();
    task_manager_t::get().add_async_callback([self, category, product, cli]()
    {
        self->nt_refresh_category_and_product_info(cli, category, product);
    });

    return {true, "ok"};
}

std::tuple<bool, std::string> google_payment_t::check_payment(const std::any& payment_identificator)
{
    auto self = google_shared_from_this();
    task_manager_t::get().add_callback([self, payment_identificator]()
    {
        try
        {
            std::any_cast<std::string>(payment_identificator);

            /*TODO SAY SERVER THAT PAYMENT HAVE BEEN DONE*/

            self->mt_send_payment_done_callback(true, "ok");
        } catch (std::exception& e)
        {
            self->mt_send_payment_done_callback(false, "bad identificator");
        }
    });

    return { true, "ok" };
}

void google_payment_t::nt_refresh_category_and_product_info(
    const std::shared_ptr<httplib::Client>& cli,
    const category_model_t& category,
    const product_model_t& product)
{
    httplib::Params params{
        {"category_id", std::to_string(category.id)},
        {"product_id", std::to_string(product.id)}
    };
    const auto result = cli->Get("/api/get_product/", params, httplib::Headers{});
    if (!nt_check_result(result, "unknown error")) return;

    const auto response = rfl::json::read<product_model_t>(result.value().body).value();

    payment_identificator_t pi;

    std::string identify;
    {
        std::random_device rd;
        auto seed_data = std::array<int, std::mt19937::state_size> {};
        std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
        std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
        std::mt19937 engine(seq);
        identify = uuids::to_string(uuids::uuid_random_generator{&engine}());
    }

    pi.identify = std::move(identify);
    pi.link = "http://www.example.com";

    /*TODO SEND IDENTIFY TO SERVER*/

    mt_send_payment_created_callback(std::move(pi), "ok");
}

bool google_payment_t::nt_check_result(const httplib::Result& result, const std::string& other_message)
{
    if (result.error() != httplib::Error::Success || result.value().status != 200)
    {
        const auto message = result
                                 ? rfl::json::read<gp_api_error_response_t>(result->body)
                                   .value_or({other_message}).message
                                 : other_message;

        auto self = google_shared_from_this();
        task_manager_t::get().add_callback([self, message]()
        {
            self->mt_send_payment_created_callback(std::nullopt, message);
        });

        return false;
    }
    return true;
}

void google_payment_t::mt_send_payment_created_callback(std::optional<payment_identificator_t>&& payment_identificator, const std::string& message)
{
    if (context_window == std::nullopt)
    {
        std::cerr << "google_payment: context window dont set" << std::endl;
    }
    else if (const auto w = context_window->lock())
    {
        if (payment_identificator == std::nullopt)
        {
            payment_created_callback(std::nullopt, message);
        }
        else
        {
            payment_created_callback(*payment_identificator, message);
        }
    }
}

void google_payment_t::mt_send_payment_done_callback(bool ok, const std::string& message)
{
    if (context_window == std::nullopt)
    {
        std::cerr << "google_payment: context window dont set" << std::endl;
    }
    else if (const auto w = context_window->lock())
    {
        payment_done_callback(ok, message);
    }
}

std::shared_ptr<google_payment_t> google_payment_t::google_shared_from_this()
{
    return std::dynamic_pointer_cast<google_payment_t>(shared_from_this());
}
