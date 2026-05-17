#include "menu_screen.hpp"

namespace menu_screen
{
    StartMenuAction hit_test(float normalizedX, float normalizedY)
    {
        auto in_rect = [normalizedX, normalizedY](float x, float y, float w, float h)
        {
            return normalizedX >= x && normalizedX <= (x + w) && normalizedY <= y && normalizedY >= (y - h);
        };

        if (in_rect(kModeX0, kModeY, kModeW, kModeH))
            return StartMenuAction::SelectCampaign;
        if (in_rect(kModeX1, kModeY, kModeW, kModeH))
            return StartMenuAction::SelectTimeAttack;
        if (in_rect(kModeX2, kModeY, kModeW, kModeH))
            return StartMenuAction::SelectCollector;
        if (in_rect(kStartX, kStartY, kStartW, kStartH))
            return StartMenuAction::Start;

        return StartMenuAction::None;
    }
}
