#pragma once
#include "gui/base_window_t.h"
#include "logic/models/category_model.h"
#include "logic/payment/base_payment.h"

class payment_window_t
    : public base_window_t
{
public:
    payment_window_t();
    ~payment_window_t() override = default;

    void update() override;

    void make_payment();

public:
    std::string payment;
    category_model_t category;
    product_model_t product;

protected:
    bool m_inited = false;
    bool m_loading = false;

    std::optional<payment_identificator_t> m_identificator = std::nullopt;

private:
    void payment_created_callback(std::optional<payment_identificator_t> payment_identificator,
                                  std::string message);

    void payment_done_callback(bool ok, std::string message);

};
