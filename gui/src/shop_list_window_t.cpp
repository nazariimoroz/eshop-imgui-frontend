#include "gui/windows/shop_list_window_t.h"

#include <functional>
#include <stdexcept>

#include "gui/gui.h"
#include "imgui.h"

shop_list_window_t::shop_list_window_t()
{
    name = "ShopListWindow";
    set_show(true);
}

void shop_list_window_t::update()
{
    if(!is_shown())
        return;

    const auto vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(vp->Size.x * 2.f/3.f, vp->Size.y));
    ImGui::Begin(name.c_str(), nullptr,
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoBringToFrontOnFocus );

    ImGui::Text("Shop");
    ImGui::Separator();

    ImGui::End();
}
