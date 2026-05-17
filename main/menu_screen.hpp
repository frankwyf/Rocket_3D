#pragma once

namespace menu_screen
{
    constexpr float kModeY = 0.45f;
    constexpr float kModeW = 0.30f;
    constexpr float kModeH = 0.16f;
    constexpr float kModeX0 = -0.50f;
    constexpr float kModeX1 = -0.15f;
    constexpr float kModeX2 = 0.20f;
    constexpr float kStartX = -0.18f;
    constexpr float kStartY = 0.10f;
    constexpr float kStartW = 0.36f;
    constexpr float kStartH = 0.18f;

    enum class StartMenuAction
    {
        None,
        SelectCampaign,
        SelectTimeAttack,
        SelectCollector,
        Start
    };

    StartMenuAction hit_test(float normalizedX, float normalizedY);
}
