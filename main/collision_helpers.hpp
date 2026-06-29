// Collision and spatial query helpers extracted from the main gameplay loop.

#ifndef COLLISION_HELPERS_HPP
#define COLLISION_HELPERS_HPP

#include <array>
#include <vector>
#include <cmath>

#include "../vmlib/vec3.hpp"

namespace collision {

inline float distance_sq(Vec3f a, Vec3f b)
{
    Vec3f d = a - b;
    return d.x * d.x + d.y * d.y + d.z * d.z;
}

inline bool within_radius(Vec3f a, Vec3f b, float radius)
{
    return distance_sq(a, b) < radius * radius;
}

struct CollisionResult
{
    bool hitObstacle = false;
    int targetsCollectedThisFrame = 0;
    int bossGatesPassedThisFrame = 0;
    bool landingZoneReached = false;
};

struct CollisionInput
{
    Vec3f rocketPos;
    float obstacleRadius;
    float targetRadius;
    float bossGateRadius;
    Vec3f landingCenter;
    float landingHalfX;
    float landingHalfY;
    float landingHalfZ;
    bool allTargetsCollected;
    bool allBossPassed;
    float flightProgress; // shootTime value
    float flightThreshold; // minimum shootTime for landing
};

inline CollisionResult evaluate(
    CollisionInput const& in,
    std::vector<Vec3f> const& obstacles,
    std::array<Vec3f, 3> const& targets,
    std::array<bool, 3> const& targetCollected,
    std::array<Vec3f, 3> const& bossGates,
    std::array<bool, 3> const& bossGatePassed)
{
    CollisionResult result{};

    for (auto const& p : obstacles)
    {
        if (within_radius(in.rocketPos, p, in.obstacleRadius))
        {
            result.hitObstacle = true;
            return result;
        }
    }

    for (std::size_t i = 0; i < targets.size(); ++i)
    {
        if (targetCollected[i])
            continue;
        if (within_radius(in.rocketPos, targets[i], in.targetRadius))
            result.targetsCollectedThisFrame++;
    }

    if (in.allTargetsCollected)
    {
        for (std::size_t i = 0; i < bossGates.size(); ++i)
        {
            if (bossGatePassed[i])
                continue;
            if (within_radius(in.rocketPos, bossGates[i], in.bossGateRadius))
                result.bossGatesPassedThisFrame++;
        }
    }

    if (in.allTargetsCollected && in.allBossPassed && in.flightProgress >= in.flightThreshold)
    {
        Vec3f d = in.rocketPos - in.landingCenter;
        if (std::fabs(d.x) < in.landingHalfX &&
            std::fabs(d.y) < in.landingHalfY &&
            std::fabs(d.z) < in.landingHalfZ)
        {
            result.landingZoneReached = true;
        }
    }

    return result;
}

} // namespace collision

#endif // COLLISION_HELPERS_HPP
