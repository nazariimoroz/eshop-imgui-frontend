#include "gui/windows/auth_window.h"

#include <iostream>
#include <regex>
#include <stdexcept>

#include "imgui.h"
#include "gui/imgui_ex.h"
#include "logic/cache.h"
#include "logic/user_fabric.h"
#include "logic/models/user_model.h"

auth_window_t::auth_window_t()
{
    name = "AuthWindow";
    set_show(true);
}

void auth_window_t::update()
{
    if (!is_shown())
        return;

    if(cache_t::get().get_user_model().lock())
    {
        set_show(false);
        return;
    }

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
    if (ImGui::BeginTabBar("##tabs"))
    {
        login_tab();

        registration_tab();

        ImGui::EndTabBar();
    }
    ImGui::End();
}


void auth_window_t::login_tab()
{
    ImGui::SetNextItemWidth(ImGui::GetColumnWidth() / 2.f);
    if (ImGui::BeginTabItem("Login"))
    {
        ImGuiEx::AlignForHeight(
            ImGui::GetFrameHeight() * 3.f +
            ImGui::GetStyle().ItemSpacing.y * 2.f);

        ImGuiEx::CentrateByX_ItemWithText(email, "Email", 100);
        ImGui::InputTextWithHint("##login_email",
                                 "Email",
                                 email,
                                 sizeof(email));

        ImGuiEx::CentrateByX_ItemWithText(password, "Password", 100);
        ImGui::InputTextWithHint("##login_password",
                                 "Password",
                                 password,
                                 sizeof(password),
                                 ImGuiInputTextFlags_Password);

        ImGuiEx::AlignForWidth(100);
        if(ImGui::Button("Login", ImVec2(100, 0)))
        {
            login();
        }

        ImGui::EndTabItem();
    }
}

void auth_window_t::registration_tab()
{
    ImGui::SetNextItemWidth(ImGui::GetColumnWidth() / 2.f);
    if (ImGui::BeginTabItem("Registration"))
    {
        ImGuiEx::AlignForHeight(
            ImGui::GetFrameHeight() * 4.f +
            ImGui::GetStyle().ItemSpacing.y * 2.f);

        ImGuiEx::CentrateByX_ItemWithText(email, "Email", 120);
        ImGui::InputTextWithHint("##reg_email",
                                 "Email",
                                 email,
                                 sizeof(email));

        ImGuiEx::CentrateByX_ItemWithText(password, "Password", 120);
        ImGui::InputTextWithHint("##reg_password",
                                 "Password",
                                 password,
                                 sizeof(password),
                                 ImGuiInputTextFlags_Password);

        ImGuiEx::CentrateByX_ItemWithText(repeat_password, "Repeat password", 120);
        ImGui::InputTextWithHint("##reg_password_repeat",
                                 "Repeat password",
                                 repeat_password,
                                 sizeof(repeat_password),
                                 ImGuiInputTextFlags_Password);

        ImGuiEx::AlignForWidth(120);
        if(ImGui::Button("Registration##1", ImVec2(120, 0)))
        {
            registration();
        }

        ImGui::EndTabItem();
    }
}

void auth_window_t::login()
{
    if(const auto ufabric = user_fabric_t::create())
    {

    }
}

void auth_window_t::registration()
{
    if(const auto ufabric = user_fabric_t::create())
    {
        const auto [ok, message] = ufabric->save(email, password, repeat_password);
        if(!ok)
        {
            std::cerr << "Failed to save: " << email << std::endl;
        }
    }
}
