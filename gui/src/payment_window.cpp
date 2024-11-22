#include "gui/windows/payment_window.h"

#include <memory>
#include <stdexcept>

#include "imgui.h"
#include "gui/gui.h"

payment_window_t::payment_window_t()
{
    name = "Payment Window";
    set_show(true);
}

void payment_window_t::update()
{
    if(!is_shown())
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

    if(ImGui::Button("Make payment##payment_window", ImVec2(ImGui::GetColumnWidth(), 0)))
    {

    }

    if(ImGui::Button("Check payment state##payment_window", ImVec2(ImGui::GetColumnWidth(), 0)))
    {

    }

    if(ImGui::Button("Close##payment_window", ImVec2(ImGui::GetColumnWidth(), 0)))
    {
        auto& gui = gui_t::get();
        context_for_correct_exit = std::dynamic_pointer_cast<payment_window_t>(gui.remove_window(this));
    }

    ImGui::End();
}
