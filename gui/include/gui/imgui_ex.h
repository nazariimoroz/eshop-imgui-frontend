#pragma once

class ImGuiEx
{
public:
    static void AlignForWidth(float width, float alignment = 0.5f);
    static void AlignForHeight(float height, float alignment = 0.5f);

    static void CentrateByX_ItemWithText(const char* buf, const char* hint, float min = 100.f);
};
