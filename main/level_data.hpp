#pragma once

#include <array>
#include <cmath>
#include <cstddef>

#include "../vmlib/vec3.hpp"

namespace level_data
{
    inline constexpr float kPi = 3.1415926f;
    inline constexpr float kMovementPerSecond = 5.f;
    inline constexpr float kMouseSensitivity = 0.01f;
    inline constexpr float kRocketSteerPerSecond = 7.f;
    inline constexpr float kLaunchSpeedMin = 0.35f;
    inline constexpr float kLaunchSpeedMax = 1.35f;
    inline constexpr float kLevelAdvanceDelay = 1.6f;
    inline constexpr float kFollowCamSmoothing = 8.0f;
    inline constexpr float kFollowCamRecentering = 1.6f;
    inline constexpr float kMaxFuel = 100.f;
    inline constexpr float kFuelBurnRate = 8.f;
    inline constexpr float kSteerFuelCost = 3.f;
    inline constexpr float kCountdownDuration = 3.f;
    inline constexpr float kMissionTimeLimit = 45.f;

    inline constexpr float kStartupGameButtonX = -0.62f;
    inline constexpr float kStartupGameButtonY = -0.75f;
    inline constexpr float kStartupButtonWidth = 0.48f;
    inline constexpr float kStartupButtonHeight = 0.20f;
    inline constexpr float kStartupTestButtonX = 0.14f;
    inline constexpr float kStartupTestButtonY = -0.75f;

    enum class GamePhase
    {
        STARTUP, COUNTDOWN, FLYING, LANDED, FAILED, PAUSED, LEVEL_CLEAR, CAMPAIGN_DONE
    };

    struct FollowCamPreset
    {
        char const* name;
        float smoothing;
        float recenter;
    };

    inline const std::array<FollowCamPreset, 3> kFollowCamPresets = {
        FollowCamPreset{ "ARCADE", 5.0f, 0.6f },
        FollowCamPreset{ "SMOOTH", kFollowCamSmoothing, kFollowCamRecentering },
        FollowCamPreset{ "PRECISION", 11.0f, 0.25f }
    };

    struct LevelConfig
    {
        char const* name;
        bool hard;
        float missionTime;
        float obstacleRadius;
        float obstacleMotionScale;
        float bossGateRadius;
    };

    inline const std::array<LevelConfig, 6> kLevels = {
        LevelConfig{ "EASY-1", false, 55.f, 1.6f, 0.65f, 2.8f },
        LevelConfig{ "EASY-2", false, 50.f, 1.8f, 0.80f, 2.5f },
        LevelConfig{ "NORMAL-1", false, 46.f, 2.0f, 1.00f, 2.2f },
        LevelConfig{ "NORMAL-2", false, 42.f, 2.1f, 1.10f, 2.0f },
        LevelConfig{ "HARD-1", true, 38.f, 2.3f, 1.25f, 1.85f },
        LevelConfig{ "HARD-2", true, 34.f, 2.5f, 1.40f, 1.65f }
    };

    inline const std::array<Vec3f, 3> kMissionTargets = {
        Vec3f{ 13.f, 4.5f, -1.f },
        Vec3f{ 30.f, 11.f, -1.f },
        Vec3f{ 52.f, 8.f, -1.f }
    };

    inline const std::array<Vec3f, 3> kObstacleBases = {
        Vec3f{ 18.f, 6.5f, -1.f },
        Vec3f{ 37.f, 9.5f, -1.f },
        Vec3f{ 55.f, 7.2f, -1.f }
    };

    inline const std::array<Vec3f, 3> kBossGateCenters = {
        Vec3f{ 58.f, 6.2f, -1.f },
        Vec3f{ 64.f, 4.7f, -1.f },
        Vec3f{ 69.f, 2.8f, -1.f }
    };

    inline std::array<Vec3f, 3> compute_obstacle_positions(float timeS, float motionScale)
    {
        std::array<Vec3f, 3> out = kObstacleBases;
        for (std::size_t i = 0; i < out.size(); ++i)
        {
            float phase = timeS * (1.1f + 0.45f * motionScale) + static_cast<float>(i) * 1.7f;
            out[i].z += std::sin(phase) * (1.6f + motionScale);
            out[i].y += std::cos(phase * 0.65f) * (0.8f + 0.6f * motionScale);
        }
        return out;
    }
}
