#include "gui/windows/product_value_window.h"

#include <format>
#include <stdexcept>

#include "gui/gui.h"
#include "gui/imgui_ex.h"

product_value_window_t::product_value_window_t()
{
    name = std::format("Payment Window N{}", m_count + 1);
    set_show(true);

    m_count += 1;
}

void product_value_window_t::update()
{
    if (!is_shown())
        return;
    [[maybe_unused]] std::shared_ptr<product_value_window_t> context_for_correct_exit = nullptr;

    const auto vp = ImGui::GetMainViewport();
    if (!m_inited)
    {
        ImGui::SetNextWindowPos(ImVec2(vp->Size.x / 4.f + m_count * 5.f, vp->Size.y / 4.f  + m_count * 5.f));
        ImGui::SetNextWindowSize(ImVec2(vp->Size.x / 4.f * 2.f, 100.f));

        m_inited = true;
    }

    ImGui::Begin(name.c_str(), nullptr,
                 ImGuiWindowFlags_NoCollapse |
                 ImGuiWindowFlags_NoResize);

    ImGui::Text("Your product:");

    ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
    ImGui::InputText(
        "##ProductValue",
        product_value.data(),
        product_value.size(),
        ImGuiInputTextFlags_ReadOnly
        | ImGuiInputTextFlags_AutoSelectAll);

    if (ImGui::Button("Close##product_value_window", ImVec2(ImGui::GetColumnWidth(), 0)))
    {
        auto& gui = gui_t::get();
        context_for_correct_exit =
            std::dynamic_pointer_cast<product_value_window_t>(gui.remove_window(this));
    }

    ImGui::End();
}


