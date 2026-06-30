// SpawnManager: procedural placement of obstacles and pickups per-level.

#ifndef SPAWN_MANAGER_HPP
#define SPAWN_MANAGER_HPP

#include <vector>
#include <cmath>

#include "../vmlib/vec3.hpp"

namespace spawn_manager {

struct SpawnConfig
{
    int obstacleCount;
    float pathStartX;
    float pathEndX;
    float lateralRange;
    float verticalBase;
    float verticalRange;
    int fuelCanisterCount;
};

inline SpawnConfig config_for_level(int level)
{
    SpawnConfig cfg{};
    cfg.obstacleCount = 3 + level * 2;
    cfg.pathStartX = 15.0f;
    cfg.pathEndX = 65.0f;
    cfg.lateralRange = 3.0f + level * 0.5f;
    cfg.verticalBase = 6.0f;
    cfg.verticalRange = 2.0f + level * 0.5f;
    cfg.fuelCanisterCount = (level < 3) ? 2 : 3;
    return cfg;
}

inline std::vector<Vec3f> generate_obstacle_positions(SpawnConfig const& cfg, int seed)
{
    std::vector<Vec3f> positions;
    positions.reserve(static_cast<std::size_t>(cfg.obstacleCount));
    float span = cfg.pathEndX - cfg.pathStartX;
    for (int i = 0; i < cfg.obstacleCount; ++i)
    {
        float t = static_cast<float>(i) / static_cast<float>(cfg.obstacleCount);
        float x = cfg.pathStartX + t * span;
        // Pseudo-random lateral offset based on seed and index
        float hash = std::sin(static_cast<float>(seed * 31 + i * 7)) * 0.5f + 0.5f;
        float z = (hash - 0.5f) * 2.0f * cfg.lateralRange;
        float yHash = std::sin(static_cast<float>(seed * 13 + i * 11)) * 0.5f + 0.5f;
        float y = cfg.verticalBase + yHash * cfg.verticalRange;
        positions.push_back(Vec3f{ x, y, z });
    }
    return positions;
}

inline std::vector<Vec3f> generate_fuel_positions(SpawnConfig const& cfg, int seed)
{
    std::vector<Vec3f> positions;
    positions.reserve(static_cast<std::size_t>(cfg.fuelCanisterCount));
    float span = cfg.pathEndX - cfg.pathStartX;
    for (int i = 0; i < cfg.fuelCanisterCount; ++i)
    {
        float t = static_cast<float>(i + 1) / static_cast<float>(cfg.fuelCanisterCount + 1);
        float x = cfg.pathStartX + t * span;
        float hash = std::sin(static_cast<float>(seed * 17 + i * 3)) * 0.5f + 0.5f;
        float z = (hash - 0.5f) * cfg.lateralRange;
        float y = cfg.verticalBase + hash * 2.0f;
        positions.push_back(Vec3f{ x, y, z });
    }
    return positions;
}

} // namespace spawn_manager

#endif // SPAWN_MANAGER_HPP
