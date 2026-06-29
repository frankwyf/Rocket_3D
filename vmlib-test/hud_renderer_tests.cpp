#include <catch2/catch_amalgamated.hpp>

#include "../main/hud_renderer.hpp"

TEST_CASE("compute_hud formats mode and level name", "[hud]")
{
    hud_renderer::HudInput hi{};
    hi.testMode = true;
    hi.levelName = "HARD-2";
    hi.maxFuel = 100.0f;
    hi.fuel = 50.0f;
    hi.totalLevels = 6;
    hi.currentLevel = 3;
    hi.camPresetName = "TIGHT";

    auto hud = hud_renderer::compute_hud(hi);
    REQUIRE(std::string(hud.line1).find("TEST") != std::string::npos);
    REQUIRE(std::string(hud.line1).find("HARD-2") != std::string::npos);
}

TEST_CASE("compute_hud calculates fuel fraction correctly", "[hud]")
{
    hud_renderer::HudInput hi{};
    hi.fuel = 40.0f;
    hi.maxFuel = 100.0f;
    hi.levelName = "LVL";
    hi.camPresetName = "X";
    hi.totalLevels = 6;
    hi.currentLevel = 2;

    auto hud = hud_renderer::compute_hud(hi);
    REQUIRE(hud.fuelFraction == Catch::Approx(0.4f));
}

TEST_CASE("compute_hud calculates level progress", "[hud]")
{
    hud_renderer::HudInput hi{};
    hi.levelName = "L";
    hi.camPresetName = "P";
    hi.totalLevels = 6;
    hi.currentLevel = 3;
    hi.maxFuel = 100.f;
    hi.fuel = 100.f;

    auto hud = hud_renderer::compute_hud(hi);
    REQUIRE(hud.levelProgress == Catch::Approx(0.5f));
}

TEST_CASE("compute_hud reports campaign cleared status", "[hud]")
{
    hud_renderer::HudInput hi{};
    hi.levelName = "L";
    hi.camPresetName = "P";
    hi.totalLevels = 6;
    hi.maxFuel = 100.f;
    hi.fuel = 80.f;
    hi.campaignCleared = true;
    hi.score = 9999;
    hi.launchCount = 12;
    hi.successfulMissions = 6;
    hi.achieveUnlocked = 5;
    hi.achieveTotal = 8;

    auto hud = hud_renderer::compute_hud(hi);
    REQUIRE(hud.hasStatus);
    REQUIRE(std::string(hud.statusLine).find("9999") != std::string::npos);
    REQUIRE(std::string(hud.statusLine).find("5/8") != std::string::npos);
}

TEST_CASE("compute_hud reports mission failed status", "[hud]")
{
    hud_renderer::HudInput hi{};
    hi.levelName = "L";
    hi.camPresetName = "P";
    hi.totalLevels = 6;
    hi.maxFuel = 100.f;
    hi.fuel = 0.f;
    hi.missionFailed = true;

    auto hud = hud_renderer::compute_hud(hi);
    REQUIRE(hud.hasStatus);
    REQUIRE(std::string(hud.statusLine).find("FAILED") != std::string::npos);
}

TEST_CASE("compute_hud no status when in progress", "[hud]")
{
    hud_renderer::HudInput hi{};
    hi.levelName = "L";
    hi.camPresetName = "P";
    hi.totalLevels = 6;
    hi.maxFuel = 100.f;
    hi.fuel = 60.f;
    hi.campaignCleared = false;
    hi.missionComplete = false;
    hi.missionFailed = false;

    auto hud = hud_renderer::compute_hud(hi);
    REQUIRE_FALSE(hud.hasStatus);
}

TEST_CASE("compute_hud wind and achievement in line7", "[hud]")
{
    hud_renderer::HudInput hi{};
    hi.levelName = "L";
    hi.camPresetName = "P";
    hi.totalLevels = 6;
    hi.maxFuel = 100.f;
    hi.fuel = 100.f;
    hi.windOffset = -1.3f;
    hi.achieveUnlocked = 2;
    hi.achieveTotal = 8;

    auto hud = hud_renderer::compute_hud(hi);
    REQUIRE(std::string(hud.line7).find("WIND") != std::string::npos);
    REQUIRE(std::string(hud.line7).find("2/8") != std::string::npos);
}

TEST_CASE("compute_hud phase shows FLIGHT when in flight", "[hud]")
{
    hud_renderer::HudInput hi{};
    hi.levelName = "L";
    hi.camPresetName = "P";
    hi.totalLevels = 6;
    hi.maxFuel = 100.f;
    hi.fuel = 100.f;
    hi.inFlight = true;
    hi.gameplayTime = 42.5f;

    auto hud = hud_renderer::compute_hud(hi);
    REQUIRE(std::string(hud.line8).find("FLIGHT") != std::string::npos);
    REQUIRE(std::string(hud.line8).find("42.5") != std::string::npos);
}
