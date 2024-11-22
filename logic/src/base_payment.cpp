#include <memory>
#include <stdexcept>
#include <logic/payment/base_payment.h>

#include "logic/payment/google_payment.h"


base_payment_t::base_payment_t(protected_t) { }

std::shared_ptr<base_payment_t> base_payment_t::create(const std::string& method)
{
    if (method == google_payment_t::name())
        return std::make_shared<google_payment_t>(protected_t{});

    return nullptr;
}
