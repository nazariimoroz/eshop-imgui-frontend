#include "gui/windows/auth_window.h"

#include <iostream>
#include <regex>
#include <stdexcept>

#include "imgui.h"
#include "imgui_internal.h"
#include "gui/gui.h"
#include "gui/imgui_ex.h"
#include "gui/windows/shop_list_window_t.h"
#include "gui/windows/user_profile_window_t.h"
#include "logic/cache.h"
#include "logic/fabrics/user_fabric.h"
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

    const auto vp = ImGui::GetMainViewport();
    if (!m_inited)
    {
        ImGui::SetNextWindowPos(ImVec2(vp->Size.x / 4.f, vp->Size.y / 4.f));
        ImGui::SetNextWindowSize(ImVec2(vp->Size.x / 4.f * 2.f, vp->Size.y / 4.f * 2.f));

        try_login_via_cache();

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

        bool pushed = false;
        if (m_loading)
        {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
            pushed = true;
        }
        {
            ImGuiEx::AlignForWidth(100);
            if(ImGui::Button("Login", ImVec2(100, 0)))
            {
                login();
            }
        }
        if (pushed)
        {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
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

        bool pushed = false;
        if (m_loading)
        {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
            pushed = true;
        }
        {
        ImGuiEx::AlignForWidth(120);
        if(ImGui::Button("Registration##1", ImVec2(120, 0)))
        {
            registration();
        }
        }
        if (pushed)
        {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
        }

        ImGui::EndTabItem();
    }
}

void auth_window_t::try_login_via_cache()
{
    m_loading = true;
    if(const auto ufabric = user_fabric_t::create())
    {
        ufabric->loaded_callback = std::bind(&auth_window_t::loaded_callback, this,
            std::placeholders::_1,
            std::placeholders::_2);
        ufabric->context_window = gui_t::get().get_window_by_name(get_name());

        const auto [ok, message] = ufabric->load_jwt(cache_t::get().get_jwt());
        if(!ok)
        {
            m_loading = false;

            std::cerr << "Failed to load: " << message << std::endl;
        }
    }
}

void auth_window_t::login()
{
    m_loading = true;
    if(const auto ufabric = user_fabric_t::create())
    {
        ufabric->loaded_callback = std::bind(&auth_window_t::loaded_callback, this,
            std::placeholders::_1,
            std::placeholders::_2);
        ufabric->context_window = gui_t::get().get_window_by_name(get_name());

        const auto [ok, message] = ufabric->load(email, password);
        if(!ok)
        {
            m_loading = false;

            std::cerr << "Failed to load: " << message << std::endl;
        }
    }
}

void auth_window_t::registration()
{
    m_loading = true;
    if(const auto ufabric = user_fabric_t::create())
    {
        ufabric->loaded_callback = std::bind(&auth_window_t::loaded_callback, this,
            std::placeholders::_1,
            std::placeholders::_2);
        ufabric->context_window = gui_t::get().get_window_by_name(get_name());

        const auto [ok, message] = ufabric->save(email, password, repeat_password);
        if(!ok)
        {
            m_loading = false;

            std::cerr << "Failed to save: " << message << std::endl;
        }
    }
}

void auth_window_t::cleanup()
{
    email[0] = 0;
    password[0] = 0;
    repeat_password[0] = 0;
}

void auth_window_t::loaded_callback(bool ok, const std::string& message)
{
    m_loading = false;
    if(ok)
    {
        gui_t::get().add_window(std::make_shared<user_profile_window_t>());
        gui_t::get().add_window(std::make_shared<shop_list_window_t>());

        set_show(false);

        cleanup();
    }
    else
    {
        std::cerr << "Load error: " << message << std::endl;
    }
}
