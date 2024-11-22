#pragma once

#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>

class base_window_t;
struct product_model_t;
struct category_model_t;

struct payment_identificator_t
{
    int payment_id;
    std::string link;
};

class base_payment_t
    : public std::enable_shared_from_this<base_payment_t>
{
protected:
    struct protected_t
    {
        explicit protected_t() = default;
    };

public:
    explicit base_payment_t(protected_t);
    virtual ~base_payment_t() = default;
    base_payment_t(const base_payment_t&) = delete;
    base_payment_t& operator=(const base_payment_t&) = delete;
    base_payment_t(base_payment_t&&) = delete;
    base_payment_t& operator=(base_payment_t&&) = delete;

    static std::shared_ptr<base_payment_t> create(const std::string& method);

    virtual std::tuple<bool, std::string> process(const category_model_t& category,
                                                  const product_model_t& product) = 0;

    virtual std::tuple<bool, std::string> check_payment(const payment_identificator_t& payment_identificator) = 0;

public:
    std::function<void(std::optional<payment_identificator_t> payment_identificator,
                       std::string message)> payment_created_callback;

    std::function<void(std::optional<std::string> product_value,
                       std::string message)> payment_done_callback;

    std::optional<std::weak_ptr<base_window_t>> context_window;
};
