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

    std::tuple<bool, std::string> check_payment(const std::any& payment_identificator) override;

protected:
    void nt_refresh_category_and_product_info(const std::shared_ptr<httplib::Client>& cli,
                                              const category_model_t& category,
                                              const product_model_t& product);

    bool nt_check_result(const httplib::Result& result, const std::string& other_message);

    void mt_send_payment_created_callback(std::optional<payment_identificator_t>&& payment_identificator,
                                          const std::string& message);

    void mt_send_payment_done_callback(bool ok, const std::string& message);

private:
    std::shared_ptr<google_payment_t> google_shared_from_this();

};
