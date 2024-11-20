#include "imgui_ex.h"

#include <stdexcept>

#include "imgui.h"

void ImGuiEx::AlignForWidth(float width, float alignment)
{
    ImGuiStyle& style = ImGui::GetStyle();
    float avail = ImGui::GetContentRegionAvail().x;
    float off = (avail - width) * alignment;
    if (off > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
}

void ImGuiEx::AlignForHeight(float height, float alignment)
{
    ImGuiStyle& style = ImGui::GetStyle();
    float avail = ImGui::GetContentRegionAvail().y;
    float off = (avail - height) * alignment;
    if (off > 0.0f)
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + off);
}

void ImGuiEx::CentrateByX_ItemWithText(const char* buf, const char* hint, float min)
{
    float buf_len = buf ? ImGui::CalcTextSize(buf).x : 0;
    float hint_len = hint ? ImGui::CalcTextSize(hint).x : 0;

    float result_len = std::max({ buf_len + 20, hint_len + 5, min});
    result_len = std::min(ImGui::GetColumnWidth(), result_len);

    ImGuiEx::AlignForWidth(result_len);
    ImGui::SetNextItemWidth(result_len);
}
