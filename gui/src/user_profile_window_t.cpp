#include "gui/windows/user_profile_window_t.h"

#include <functional>
#include <iostream>
#include <stdexcept>

#include "gui/gui.h"
#include "imgui.h"
#include "logic/cache.h"
#include "logic/models/user_model.h"

user_profile_window_t::user_profile_window_t()
{
    name = "UserProfile";
    set_show(true);
}

void user_profile_window_t::update()
{
    if(!is_shown())
        return;

    const auto vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(vp->Size.x * 2.f / 3.f, 0));
    ImGui::SetNextWindowSize(ImVec2(vp->Size.x / 3.f, vp->Size.y));
    ImGui::Begin(name.c_str(), nullptr,
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoBringToFrontOnFocus );

    if(const auto user = cache_t::get().get_user_model().lock())
    {
        ImGui::Text(user->email.value().c_str());
        ImGui::Separator();

        ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0);
        if (ImGui::TreeNode("Your payments"))
        {
            ImGui::Button("Payment3", ImVec2(ImGui::GetColumnWidth(), 0));
            ImGui::TreePop();
        }
        else if (ImGui::TreeNode("Change your info"))
        {
            ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
            ImGui::InputTextWithHint("##new_email",
                "New email(optional)",
                new_email,
                256);

            ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
            ImGui::InputTextWithHint("##new_password",
                "New password(optional)",
                new_password,
                256, ImGuiInputTextFlags_Password);

            if(new_password[0])
            {
                ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
                ImGui::InputTextWithHint("##new_password_repeat",
                    "Repeat password",
                    new_password_repeat,
                    256, ImGuiInputTextFlags_Password);
            }

            if(new_email[0] || new_password[0])
            {
                if(!new_password[0] || (new_password[0] && strcmp(new_password, new_password_repeat) == 0))
                {
                    ImGui::Spacing();
                    ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
                    ImGui::InputTextWithHint("##old_password",
                        "Old password",
                        old_password,
                        256, ImGuiInputTextFlags_Password);


                    ImGui::Button("Change password", ImVec2(ImGui::GetColumnWidth(), 0));
                }
            }

            ImGui::TreePop();
        }
        else
        {
            if(ImGui::Button("Logout", ImVec2(ImGui::GetColumnWidth(), 0)))
            {
                logout();
            }
        }

        ImGui::PopStyleVar();
    }

    ImGui::End();
}

void user_profile_window_t::logout()
{
    auto& cache = cache_t::get();
    cache.set_jwt("");
    cache.set_user_model(nullptr);

    auto& gui = gui_t::get();
    if(const auto aw = gui.get_window_by_name("AuthWindow").lock())
    {
        aw->set_show(true);
    }
}
