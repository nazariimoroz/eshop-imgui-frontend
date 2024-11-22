#pragma once
#include <cstdint>

class ImGuiEx
{
public:
    static void AlignForWidth(float width, float alignment = 0.5f);
    static void AlignForHeight(float height, float alignment = 0.5f);

    static void CentrateByX_ItemWithText(const char* buf, const char* hint, float min = 100.f);

    static bool Spinner(const char* label, float radius, int thickness, const uint32_t& color);
};
