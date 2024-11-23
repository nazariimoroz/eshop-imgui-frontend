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

    void product_ui(std::shared_ptr<payment_window_t>& context_for_correct_exit);
    void payment_ui(std::shared_ptr<payment_window_t>& context_for_correct_exit);

    void make_payment();
    void check_payment();

public:
    std::string payment;
    category_model_t category;
    product_model_t product;

protected:
    bool m_inited = false;
    bool m_loading = false;

    std::optional<payment_identificator_t> m_identificator = std::nullopt;
    std::optional<std::string> m_product_value = std::nullopt;

private:
    void payment_created_callback(std::optional<payment_identificator_t> payment_identificator,
                                  std::string message);

    void payment_done_callback(std::optional<std::string> product_value,
                               std::string message);

    void open_payment_link();
};
