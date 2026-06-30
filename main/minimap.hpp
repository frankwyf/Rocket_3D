// MiniMap data provider: computes 2D projected positions for HUD mini-map overlay.

#ifndef MINIMAP_HPP
#define MINIMAP_HPP

#include <array>
#include <vector>
#include <cmath>

#include "../vmlib/vec3.hpp"

namespace minimap {

struct MapPoint
{
    float u; // 0..1 horizontal
    float v; // 0..1 vertical
    enum class Kind { ROCKET, TARGET, BOSS_GATE, OBSTACLE, FUEL, LANDING } kind;
};

struct MapConfig
{
    float worldMinX;
    float worldMaxX;
    float worldMinZ;
    float worldMaxZ;
};

inline float remap(float value, float inMin, float inMax)
{
    if (inMax <= inMin) return 0.5f;
    float t = (value - inMin) / (inMax - inMin);
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    return t;
}

struct MapInput
{
    Vec3f rocketPos;
    std::array<Vec3f, 3> targets;
    std::array<bool, 3> targetCollected;
    std::array<Vec3f, 3> bossGates;
    std::array<bool, 3> bossGatePassed;
    Vec3f landingPos;
};

inline std::vector<MapPoint> compute_map(MapConfig const& cfg, MapInput const& in)
{
    std::vector<MapPoint> pts;
    pts.reserve(10);

    auto project = [&](Vec3f p, MapPoint::Kind k) {
        MapPoint mp;
        mp.u = remap(p.x, cfg.worldMinX, cfg.worldMaxX);
        mp.v = remap(p.z, cfg.worldMinZ, cfg.worldMaxZ);
        mp.kind = k;
        pts.push_back(mp);
    };

    project(in.rocketPos, MapPoint::Kind::ROCKET);

    for (std::size_t i = 0; i < in.targets.size(); ++i)
    {
        if (!in.targetCollected[i])
            project(in.targets[i], MapPoint::Kind::TARGET);
    }

    for (std::size_t i = 0; i < in.bossGates.size(); ++i)
    {
        if (!in.bossGatePassed[i])
            project(in.bossGates[i], MapPoint::Kind::BOSS_GATE);
    }

    project(in.landingPos, MapPoint::Kind::LANDING);

    return pts;
}

} // namespace minimap

#endif // MINIMAP_HPP
