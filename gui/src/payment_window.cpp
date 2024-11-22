#include "gui/windows/payment_window.h"

#include <memory>
#include <stdexcept>

#include "imgui.h"
#include "imgui_internal.h"
#include "gui/gui.h"
#include "gui/imgui_ex.h"

payment_window_t::payment_window_t()
{
    name = "Payment Window";
    set_show(true);
}

void payment_window_t::update()
{
    if (!is_shown())
        return;
    [[maybe_unused]] std::shared_ptr<payment_window_t> context_for_correct_exit = nullptr;

    const auto vp = ImGui::GetMainViewport();
    if (!m_inited)
    {
        ImGui::SetNextWindowPos(ImVec2(vp->Size.x / 4.f, vp->Size.y / 4.f));
        ImGui::SetNextWindowSize(ImVec2(vp->Size.x / 4.f * 2.f, vp->Size.y / 4.f * 2.f));

        m_inited = true;
    }

    ImGui::Begin(name.c_str(), nullptr,
                 ImGuiWindowFlags_NoCollapse |
                 ImGuiWindowFlags_NoResize);

    const std::string amount = (product.amount.get() == -1)
                                   ? "infinity"
                                   : std::to_string(product.amount.get());

    ImGui::Text("Category: %s", category.name.c_str());
    ImGui::Text("Payment: %s", payment.c_str());
    ImGui::Text("Title: %s", product.name.c_str());
    ImGui::Text("Description:\n%s", product.description.c_str());
    ImGui::Text("Amount: %s", amount.c_str());
    ImGui::Text("Price: %i", product.price);

    bool pushed = false;
    if (m_loading)
    {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        pushed = true;
    }

    if (m_identificator)
    {
        ImGui::Text("Waiting for your payment:  %c", "|/-\\"[(int)(ImGui::GetTime() / 0.05f) & 3]);
    }

    {
        if (ImGui::Button("Make payment##payment_window", ImVec2(ImGui::GetColumnWidth(), 0)))
        {
            make_payment();
        }

        bool pushed2 = false;
        if (!m_identificator)
        {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
            pushed2 = true;
        }
        {
            if (ImGui::Button("Check payment state##payment_window", ImVec2(ImGui::GetColumnWidth(), 0)))
            {
                check_payment();
            }
        }
        if (pushed2)
        {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
        }

        if (ImGui::Button("Close##payment_window", ImVec2(ImGui::GetColumnWidth(), 0)))
        {
            auto& gui = gui_t::get();
            context_for_correct_exit = std::dynamic_pointer_cast<payment_window_t>(gui.remove_window(this));
        }
    }

    if (pushed)
    {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
    }

    ImGui::End();
}

void payment_window_t::make_payment()
{
    if (m_identificator)
    {
        open_payment_link();
        return;
    }

    if (auto p = base_payment_t::create(payment))
    {
        using namespace std::placeholders;

        p->payment_created_callback = std::bind(
            &payment_window_t::payment_created_callback,
            this,
            _1, _2);

        p->context_window = gui_t::get().get_window_by_name(get_name());

        const auto [ok, message] = p->process(category, product);
        if (!ok)
        {
            std::cerr << message << std::endl;
            return;
        }

        m_loading = true;
    }
}

void payment_window_t::check_payment()
{
    if (!m_identificator)
        return;

    if (auto p = base_payment_t::create(payment))
    {
        using namespace std::placeholders;

        p->payment_done_callback = std::bind(
            &payment_window_t::payment_done_callback,
            this,
            _1, _2);

        p->context_window = gui_t::get().get_window_by_name(get_name());

        const auto [ok, message] = p->check_payment(*m_identificator);
        if (!ok)
        {
            std::cerr << message << std::endl;
            return;
        }

        m_loading = true;
    }
}

void payment_window_t::payment_created_callback(
    std::optional<payment_identificator_t> payment_identificator,
    std::string message)
{
    m_loading = false;

    m_identificator = std::move(payment_identificator);
    if (m_identificator == std::nullopt)
    {
        std::cerr << message << std::endl;
        return;
    }

    open_payment_link();
}

void payment_window_t::payment_done_callback(
    std::optional<std::string> product_value,
    std::string message)
{
    m_loading = false;

    if (!product_value)
    {
        std::cerr << message << std::endl;
        return;
    }

    std::cout << *product_value << std::endl;
    /** TODO Open product value window */
}

void payment_window_t::open_payment_link()
{
    if (m_identificator)
    {
        ShellExecuteA(NULL,
            "open",
            m_identificator->link.c_str(),
            NULL,
            NULL,
            SW_SHOWNORMAL);
    }
}
