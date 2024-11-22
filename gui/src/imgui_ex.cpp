#include "gui/imgui_ex.h"

#include <stdexcept>

#include "imgui.h"
#include "imgui_internal.h"

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

bool ImGuiEx::Spinner(const char* label, float radius, int thickness, const uint32_t& color)
{
    using namespace ImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size((radius )*2, (radius + style.FramePadding.y)*2);

    const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
    ItemSize(bb, style.FramePadding.y);
    if (!ItemAdd(bb, id))
        return false;

    // Render
    window->DrawList->PathClear();

    int num_segments = 30;
    int start = abs(ImSin(g.Time*1.8f)*(num_segments-5));

    const float a_min = IM_PI*2.0f * ((float)start) / (float)num_segments;
    const float a_max = IM_PI*2.0f * ((float)num_segments-3) / (float)num_segments;

    const ImVec2 centre = ImVec2(pos.x+radius, pos.y+radius+style.FramePadding.y);

    for (int i = 0; i < num_segments; i++) {
        const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
        window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a+g.Time*8) * radius,
                                            centre.y + ImSin(a+g.Time*8) * radius));
    }

    window->DrawList->PathStroke(color, false, thickness);
}
