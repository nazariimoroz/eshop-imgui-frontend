#pragma once
#include <httplib.h>

#include "base_payment.h"

namespace httplib
{
    class Client;
}

class google_payment_t
    : public base_payment_t
{
public:
    explicit google_payment_t(protected_t);

    static const char* name();

    std::tuple<bool, std::string> process(const category_model_t& category, const product_model_t& product) override;

protected:
    void nt_create_payment(const std::shared_ptr<httplib::Client>& cli,
                           const category_model_t& category,
                           const product_model_t& local_product, const std::string& jwt);

    std::optional<product_model_t> nt_refresh_product_info(const std::shared_ptr<httplib::Client>& cli,
                                                           const category_model_t& category,
                                                           const product_model_t& product);

    bool nt_check_req_result_for_create_payment(const httplib::Result& result,
                                                const std::string& other_message);

    void mt_send_payment_created_callback(std::optional<payment_identificator_t>&& payment_identificator,
                                          const std::string& message);

public:
    std::tuple<bool, std::string> check_payment(
        const payment_identificator_t& payment_identificator) override;

protected:
    void nt_check_payment(const payment_identificator_t& payment_identificator,
                          const std::string& jwt,
                          const std::shared_ptr<httplib::Client>& cli);

    bool nt_check_req_result_for_check_payment(const httplib::Result& result,
                                               const std::string& other_message);

    void mt_send_payment_done_callback(std::optional<std::string> product_value,
                                       const std::string& message);


private:
    std::shared_ptr<google_payment_t> google_shared_from_this();
};
