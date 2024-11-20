#include "shop_list_window.h"

#include <functional>
#include <stdexcept>

#include "gui.h"
#include "imgui.h"

shop_list_window::shop_list_window()
{
    name = "ShopListWindow";
    set_show(true);
}

void shop_list_window::update()
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

    if (ImGui::BeginTabBar("#tabs"))
    {
        if (ImGui::BeginTabItem("Esp"))
        {
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Aim"))
        {
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Config"))
        {
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}
