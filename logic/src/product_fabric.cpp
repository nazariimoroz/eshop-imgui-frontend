#include "logic/fabrics/product_fabric.h"

#include "../../.env.h"
#include <httplib.h>
#include <stdexcept>
#include <rfl/json/read.hpp>

#include "logic/models/category_model.h"
#include "logic/utilities/task_manager.h"

struct pf_api_error_response_t
{
    std::string message;
};

product_fabric_t::product_fabric_t(private_t)
{
}

product_fabric_t::~product_fabric_t() = default;

std::tuple<bool, std::string> product_fabric_t::load_all() const
{
    auto cli = std::make_shared<httplib::Client>(BACKEND_HOST);
    if (!cli->is_valid())
    {
        return {false, "Cant open server"};
    }

    auto self = shared_from_this();
    task_manager_t::get().add_async_callback([self, cli]()
    {
        const auto result = cli->Get("/api/get_all_products");

        if (result.error() != httplib::Error::Success || result.value().status != 200)
        {
            const auto message = result
                                     ? rfl::json::read<pf_api_error_response_t>(result->body)
                                       .value_or({"unknown error"}).message
                                     : "unknown error";

            self->send_callback(std::nullopt, message);
            return;
        }

        const auto response = rfl::json::read<ctg_models_vec_t>(result.value().body).value();

        self->send_callback(response, "ok");
    });

    return {true, "ok"};
}

void product_fabric_t::send_callback(const std::optional<ctg_models_vec_t>& models, const std::string& message) const
{
    if(loaded_callback)
    {
        task_manager_t::get().add_callback([self = shared_from_this(), models, message]()
        {
            if(self->context_window == std::nullopt)
            {
                std::cerr << "user_fabric: context window dont set" << std::endl;
            }
            else if (const auto w = self->context_window->lock())
            {
                self->loaded_callback(models, message);
            }
        });
    }
}
