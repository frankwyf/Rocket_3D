#include <catch2/catch_amalgamated.hpp>

#include "../main/menu_screen.hpp"

TEST_CASE("Start menu hit-test handles boundary-inclusive rectangles", "[menu][boundary]")
{
    using menu_screen::StartMenuAction;

    SECTION("Campaign top-left corner")
    {
        auto action = menu_screen::hit_test(menu_screen::kModeX0, menu_screen::kModeY);
        REQUIRE(action == StartMenuAction::SelectCampaign);
    }

    SECTION("Collector bottom-right corner")
    {
        auto action = menu_screen::hit_test(
            menu_screen::kModeX2 + menu_screen::kModeW,
            menu_screen::kModeY - menu_screen::kModeH
        );
        REQUIRE(action == StartMenuAction::SelectCollector);
    }

    SECTION("Start button edge")
    {
        auto action = menu_screen::hit_test(menu_screen::kStartX + menu_screen::kStartW, menu_screen::kStartY);
        REQUIRE(action == StartMenuAction::Start);
    }
}

TEST_CASE("Start menu hit-test rejects points just outside controls", "[menu][boundary]")
{
    using menu_screen::StartMenuAction;

    auto eps = 0.0001f;

    auto outsideCampaign = menu_screen::hit_test(menu_screen::kModeX0 - eps, menu_screen::kModeY);
    auto outsideStart = menu_screen::hit_test(menu_screen::kStartX + menu_screen::kStartW + eps, menu_screen::kStartY);

    REQUIRE(outsideCampaign == StartMenuAction::None);
    REQUIRE(outsideStart == StartMenuAction::None);
}
