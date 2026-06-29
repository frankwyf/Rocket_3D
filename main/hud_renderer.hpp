// Provides HUD data computation independent of rendering, allowing unit testing
// of the textual output without OpenGL context.

#ifndef HUD_RENDERER_HPP
#define HUD_RENDERER_HPP

#include <cstdio>
#include <string>
#include <array>

namespace hud_renderer {

struct HudData
{
    char line1[128];
    char line2[128];
    char line3[128];
    char line4[128];
    char line5[128];
    char line6[128];
    char line7[128];
    char line8[128];
    char statusLine[128];
    bool hasStatus;

    float fuelFraction;
    float levelProgress;
};

struct HudInput
{
    bool testMode;
    char const* levelName;
    int score;
    int successfulMissions;
    int launchCount;
    int collectedCount;
    int bossPassedCount;
    float missionTimer;
    float launchSpeedScale;
    bool followCamera;
    float fuel;
    float maxFuel;
    char const* camPresetName;
    float followCamSmoothing;
    float followCamRecentering;
    int comboCount;
    int fuelRemaining;
    int fuelTotal;
    float windOffset;
    int achieveUnlocked;
    int achieveTotal;
    float gameplayTime;
    bool inFlight;
    int currentLevel;
    int totalLevels;

    bool campaignCleared;
    bool missionComplete;
    bool missionFailed;
};

inline HudData compute_hud(const HudInput& in)
{
    HudData d{};

    std::snprintf(d.line1, sizeof(d.line1), "MODE: %s  LEVEL: %s",
        in.testMode ? "TEST" : "GAME", in.levelName);
    std::snprintf(d.line2, sizeof(d.line2), "SCORE: %d  MISSIONS: %d  LAUNCHES: %d",
        in.score, in.successfulMissions, in.launchCount);
    std::snprintf(d.line3, sizeof(d.line3), "TARGETS: %d/3  BOSS: %d/3  TIMER: %.1fs",
        in.collectedCount, in.bossPassedCount, in.missionTimer);
    std::snprintf(d.line4, sizeof(d.line4), "SPEED: x%.2f  CAM: %s  FUEL: %.0f%%",
        in.launchSpeedScale, in.followCamera ? "ON" : "OFF", in.fuel);
    std::snprintf(d.line5, sizeof(d.line5), "CAM %s  S/R: %.1f / %.1f  (P, ,/. ;/')",
        in.camPresetName, in.followCamSmoothing, in.followCamRecentering);
    std::snprintf(d.line6, sizeof(d.line6), "COMBO: x%d  CANISTERS: %d/%d",
        in.comboCount, in.fuelRemaining, in.fuelTotal);
    std::snprintf(d.line7, sizeof(d.line7), "WIND: %+.1f  ACHIEVE: %d/%d",
        in.windOffset, in.achieveUnlocked, in.achieveTotal);
    std::snprintf(d.line8, sizeof(d.line8), "TIME: %.1fs  PHASE: %s",
        in.gameplayTime, in.inFlight ? "FLIGHT" : "IDLE");

    d.fuelFraction = (in.maxFuel > 0.f) ? (in.fuel / in.maxFuel) : 0.f;
    d.levelProgress = (in.totalLevels > 0) ? static_cast<float>(in.currentLevel) / static_cast<float>(in.totalLevels) : 0.f;

    d.hasStatus = false;
    if (in.campaignCleared)
    {
        std::snprintf(d.statusLine, sizeof(d.statusLine),
            "FINAL SCORE: %d  LAUNCHES: %d  MISSIONS: %d/6  ACHIEVE: %d/%d",
            in.score, in.launchCount, in.successfulMissions, in.achieveUnlocked, in.achieveTotal);
        d.hasStatus = true;
    }
    else if (in.missionComplete)
    {
        std::snprintf(d.statusLine, sizeof(d.statusLine), "MISSION COMPLETE - AUTO NEXT LEVEL");
        d.hasStatus = true;
    }
    else if (in.missionFailed)
    {
        std::snprintf(d.statusLine, sizeof(d.statusLine), "MISSION FAILED - PRESS R TO RETRY");
        d.hasStatus = true;
    }

    return d;
}

} // namespace hud_renderer

#endif // HUD_RENDERER_HPP
