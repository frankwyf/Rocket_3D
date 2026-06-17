#include <catch2/catch_amalgamated.hpp>

#include "../main/menu_screen.hpp"
#include "../main/menu_screen.cpp"
#include "../main/game_config.hpp"

TEST_CASE("Start menu hit testing returns expected actions", "[menu][input]")
{
    using menu_screen::StartMenuAction;

    SECTION("Campaign button")
    {
        auto action = menu_screen::hit_test(
            menu_screen::kModeX0 + menu_screen::kModeW * 0.5f,
            menu_screen::kModeY - menu_screen::kModeH * 0.5f
        );
        REQUIRE(action == StartMenuAction::SelectCampaign);
    }

    SECTION("Time attack button")
    {
        auto action = menu_screen::hit_test(
            menu_screen::kModeX1 + menu_screen::kModeW * 0.5f,
            menu_screen::kModeY - menu_screen::kModeH * 0.5f
        );
        REQUIRE(action == StartMenuAction::SelectTimeAttack);
    }

    SECTION("Collector button")
    {
        auto action = menu_screen::hit_test(
            menu_screen::kModeX2 + menu_screen::kModeW * 0.5f,
            menu_screen::kModeY - menu_screen::kModeH * 0.5f
        );
        REQUIRE(action == StartMenuAction::SelectCollector);
    }

    SECTION("Start button")
    {
        auto action = menu_screen::hit_test(
            menu_screen::kStartX + menu_screen::kStartW * 0.5f,
            menu_screen::kStartY - menu_screen::kStartH * 0.5f
        );
        REQUIRE(action == StartMenuAction::Start);
    }

    SECTION("Outside buttons")
    {
        auto action = menu_screen::hit_test(0.95f, -0.95f);
        REQUIRE(action == StartMenuAction::None);
    }
}

TEST_CASE("Game configuration reset restores defaults", "[config]")
{
    GameConfig config;

    config.getGraphics().screenWidth = 1920;
    config.getGraphics().fullscreen = true;
    config.getGameplay().difficulty = 2.0f;
    config.getAudio().masterVolume = 0.25f;
    config.getInput().invertMouseY = true;

    config.resetToDefaults();

    REQUIRE(config.getGraphics().screenWidth == 1280);
    REQUIRE(config.getGraphics().screenHeight == 720);
    REQUIRE(config.getGraphics().fullscreen == false);
    REQUIRE(config.getGraphics().vsync == true);

    REQUIRE(config.getGameplay().difficulty == 1.0f);
    REQUIRE(config.getGameplay().rocketSpeed == 5.0f);

    REQUIRE(config.getAudio().masterVolume == 1.0f);
    REQUIRE(config.getAudio().audioEnabled == true);

    REQUIRE(config.getInput().invertMouseY == false);
    REQUIRE(config.getInput().mouseSensitivity == 0.01f);
}
