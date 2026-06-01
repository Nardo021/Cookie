#pragma once
#include <ImGui/imgui.h>

// Lucide cookie icon (https://lucide.dev/icons/cookie)
// Outline tessellated from SVG path data.
namespace CookieIcon
{
    inline constexpr ImVec2 kOutline[] = {
        { 12.0f, 2.0f },
        { 10.994f, 2.051f },
        { 9.999f, 2.202f },
        { 9.024f, 2.453f },
        { 8.079f, 2.801f },
        { 7.174f, 3.242f },
        { 6.317f, 3.772f },
        { 5.519f, 4.385f },
        { 4.786f, 5.075f },
        { 4.126f, 5.836f },
        { 3.546f, 6.659f },
        { 3.052f, 7.536f },
        { 2.648f, 8.459f },
        { 2.339f, 9.417f },
        { 2.129f, 10.402f },
        { 2.018f, 11.402f },
        { 2.008f, 12.409f },
        { 2.1f, 13.412f },
        { 2.292f, 14.4f },
        { 2.583f, 15.364f },
        { 2.969f, 16.294f },
        { 3.447f, 17.181f },
        { 4.011f, 18.015f },
        { 4.656f, 18.788f },
        { 5.376f, 19.492f },
        { 6.163f, 20.12f },
        { 7.009f, 20.666f },
        { 7.906f, 21.124f },
        { 8.844f, 21.489f },
        { 9.815f, 21.758f },
        { 10.807f, 21.929f },
        { 11.812f, 21.998f },
        { 12.818f, 21.966f },
        { 13.816f, 21.834f },
        { 14.796f, 21.601f },
        { 15.747f, 21.271f },
        { 16.66f, 20.848f },
        { 17.527f, 20.334f },
        { 18.337f, 19.736f },
        { 19.083f, 19.06f },
        { 19.757f, 18.312f },
        { 20.352f, 17.5f },
        { 20.863f, 16.632f },
        { 21.284f, 15.717f },
        { 21.61f, 14.765f },
        { 21.84f, 13.784f },
        { 21.969f, 12.786f },
        { 21.787f, 12.059f },
        { 20.789f, 12.177f },
        { 19.794f, 12.042f },
        { 18.863f, 11.664f },
        { 18.056f, 11.066f },
        { 17.423f, 10.285f },
        { 17.005f, 9.372f },
        { 16.828f, 8.383f },
        { 16.903f, 7.381f },
        { 16.402f, 7.135f },
        { 15.397f, 7.154f },
        { 14.419f, 6.923f },
        { 13.53f, 6.455f },
        { 12.786f, 5.78f },
        { 12.233f, 4.941f },
        { 11.907f, 3.991f },
        { 11.827f, 2.99f },
        { 12.0f, 2.0f },
    };

    inline constexpr ImVec2 kChips[] = {
        { 8.5f, 8.5f },
        { 16.0f, 15.5f },
        { 12.0f, 12.0f },
        { 11.0f, 17.0f },
        { 7.0f, 14.0f },
    };

    inline void DrawLucideCookie(ImDrawList* drawList, ImVec2 topLeft, float size, ImU32 color, float thickness = 1.5f)
    {
        if (!drawList || size <= 0.0f)
            return;

        const float scale = size / 24.0f;
        const int outlineCount = IM_ARRAYSIZE(kOutline);

        ImVec2 outlinePoints[IM_ARRAYSIZE(kOutline)];
        for (int i = 0; i < outlineCount; ++i)
        {
            outlinePoints[i] = ImVec2(
                topLeft.x + kOutline[i].x * scale,
                topLeft.y + kOutline[i].y * scale);
        }

        drawList->AddPolyline(outlinePoints, outlineCount, color, ImDrawFlags_Closed, thickness);

        const float chipRadius = 1.1f * scale;
        for (const ImVec2& chip : kChips)
        {
            drawList->AddCircleFilled(
                ImVec2(topLeft.x + chip.x * scale, topLeft.y + chip.y * scale),
                chipRadius,
                color);
        }
    }
}
