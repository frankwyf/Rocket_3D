#include <catch2/catch_amalgamated.hpp>

#include "../main/level_data.hpp"

TEST_CASE("Level table has 6 entries with increasing difficulty", "[level_data][levels]")
{
    REQUIRE(level_data::kLevels.size() == 6);
    REQUIRE_FALSE(level_data::kLevels.front().hard);
    REQUIRE(level_data::kLevels.back().hard);

    for (std::size_t i = 1; i < level_data::kLevels.size(); ++i)
    {
        REQUIRE(level_data::kLevels[i].missionTime < level_data::kLevels[i - 1].missionTime);
        REQUIRE(level_data::kLevels[i].obstacleRadius >= level_data::kLevels[i - 1].obstacleRadius);
    }
}

TEST_CASE("Obstacle positions differ from base positions over time", "[level_data][obstacles]")
{
    auto atZero = level_data::compute_obstacle_positions(0.0f, 1.0f);
    auto atFive = level_data::compute_obstacle_positions(5.0f, 1.0f);

    bool anyDiff = false;
    for (std::size_t i = 0; i < atZero.size(); ++i)
    {
        if (std::fabs(atZero[i].z - atFive[i].z) > 0.001f ||
            std::fabs(atZero[i].y - atFive[i].y) > 0.001f)
        {
            anyDiff = true;
        }
    }
    REQUIRE(anyDiff);
}

TEST_CASE("Obstacle motionScale amplifies displacement", "[level_data][obstacles]")
{
    float t = 2.5f;
    auto low = level_data::compute_obstacle_positions(t, 0.0f);
    auto high = level_data::compute_obstacle_positions(t, 2.0f);

    float dispLow = 0.0f;
    float dispHigh = 0.0f;
    for (std::size_t i = 0; i < low.size(); ++i)
    {
        float dz = low[i].z - level_data::kObstacleBases[i].z;
        float dy = low[i].y - level_data::kObstacleBases[i].y;
        dispLow += dz * dz + dy * dy;

        float dz2 = high[i].z - level_data::kObstacleBases[i].z;
        float dy2 = high[i].y - level_data::kObstacleBases[i].y;
        dispHigh += dz2 * dz2 + dy2 * dy2;
    }
    REQUIRE(dispHigh > dispLow);
}

TEST_CASE("Follow cam presets table has valid entries", "[level_data][cam]")
{
    REQUIRE(level_data::kFollowCamPresets.size() == 3);
    for (const auto& preset : level_data::kFollowCamPresets)
    {
        REQUIRE(preset.smoothing > 0.0f);
        REQUIRE(preset.recenter >= 0.0f);
        REQUIRE(preset.name != nullptr);
    }
}

TEST_CASE("Mission targets and boss gates are spatially ordered", "[level_data][targets]")
{
    for (std::size_t i = 1; i < level_data::kMissionTargets.size(); ++i)
    {
        REQUIRE(level_data::kMissionTargets[i].x > level_data::kMissionTargets[i - 1].x);
    }

    for (std::size_t i = 1; i < level_data::kBossGateCenters.size(); ++i)
    {
        REQUIRE(level_data::kBossGateCenters[i].x > level_data::kBossGateCenters[i - 1].x);
    }
}
