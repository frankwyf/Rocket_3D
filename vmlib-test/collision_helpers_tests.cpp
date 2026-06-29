#include <catch2/catch_amalgamated.hpp>

#include "../main/collision_helpers.hpp"

TEST_CASE("collision::distance_sq computes squared distance", "[collision]")
{
    Vec3f a{ 1.0f, 2.0f, 3.0f };
    Vec3f b{ 4.0f, 6.0f, 3.0f };
    // (3^2 + 4^2 + 0) = 25
    REQUIRE(collision::distance_sq(a, b) == Catch::Approx(25.0f));
}

TEST_CASE("collision::within_radius detects proximity", "[collision]")
{
    Vec3f a{ 0.0f, 0.0f, 0.0f };
    Vec3f b{ 1.0f, 0.0f, 0.0f };
    REQUIRE(collision::within_radius(a, b, 1.5f));
    REQUIRE_FALSE(collision::within_radius(a, b, 0.5f));
}

TEST_CASE("collision::evaluate detects obstacle hit", "[collision]")
{
    collision::CollisionInput in{};
    in.rocketPos = Vec3f{ 5.0f, 0.0f, 0.0f };
    in.obstacleRadius = 2.0f;
    in.targetRadius = 1.5f;
    in.bossGateRadius = 2.0f;
    in.landingCenter = Vec3f{ 71.0f, -0.97f, -1.0f };
    in.landingHalfX = 1.8f;
    in.landingHalfY = 1.2f;
    in.landingHalfZ = 1.5f;

    std::vector<Vec3f> obstacles = { Vec3f{ 5.5f, 0.0f, 0.0f } };
    std::array<Vec3f, 3> targets = { Vec3f{10,0,0}, Vec3f{20,0,0}, Vec3f{30,0,0} };
    std::array<bool, 3> tCol = { false, false, false };
    std::array<Vec3f, 3> boss = { Vec3f{40,0,0}, Vec3f{50,0,0}, Vec3f{60,0,0} };
    std::array<bool, 3> bPass = { false, false, false };

    auto result = collision::evaluate(in, obstacles, targets, tCol, boss, bPass);
    REQUIRE(result.hitObstacle);
    REQUIRE(result.targetsCollectedThisFrame == 0);
}

TEST_CASE("collision::evaluate detects target collection", "[collision]")
{
    collision::CollisionInput in{};
    in.rocketPos = Vec3f{ 10.0f, 0.0f, 0.0f };
    in.obstacleRadius = 1.0f;
    in.targetRadius = 1.5f;
    in.bossGateRadius = 2.0f;
    in.landingCenter = Vec3f{ 71.0f, -0.97f, -1.0f };
    in.landingHalfX = 1.8f;
    in.landingHalfY = 1.2f;
    in.landingHalfZ = 1.5f;

    std::vector<Vec3f> obstacles = { Vec3f{ 50.0f, 50.0f, 50.0f } };
    std::array<Vec3f, 3> targets = { Vec3f{10.5f,0,0}, Vec3f{20,0,0}, Vec3f{30,0,0} };
    std::array<bool, 3> tCol = { false, false, false };
    std::array<Vec3f, 3> boss = { Vec3f{40,0,0}, Vec3f{50,0,0}, Vec3f{60,0,0} };
    std::array<bool, 3> bPass = { false, false, false };

    auto result = collision::evaluate(in, obstacles, targets, tCol, boss, bPass);
    REQUIRE_FALSE(result.hitObstacle);
    REQUIRE(result.targetsCollectedThisFrame == 1);
}

TEST_CASE("collision::evaluate skips already collected targets", "[collision]")
{
    collision::CollisionInput in{};
    in.rocketPos = Vec3f{ 10.0f, 0.0f, 0.0f };
    in.obstacleRadius = 1.0f;
    in.targetRadius = 1.5f;
    in.bossGateRadius = 2.0f;
    in.landingCenter = Vec3f{ 71.0f, -0.97f, -1.0f };
    in.landingHalfX = 1.8f;
    in.landingHalfY = 1.2f;
    in.landingHalfZ = 1.5f;

    std::vector<Vec3f> obstacles;
    std::array<Vec3f, 3> targets = { Vec3f{10.0f,0,0}, Vec3f{20,0,0}, Vec3f{30,0,0} };
    std::array<bool, 3> tCol = { true, false, false };
    std::array<Vec3f, 3> boss = { Vec3f{40,0,0}, Vec3f{50,0,0}, Vec3f{60,0,0} };
    std::array<bool, 3> bPass = { false, false, false };

    auto result = collision::evaluate(in, obstacles, targets, tCol, boss, bPass);
    REQUIRE(result.targetsCollectedThisFrame == 0);
}

TEST_CASE("collision::evaluate detects boss gate passage", "[collision]")
{
    collision::CollisionInput in{};
    in.rocketPos = Vec3f{ 40.0f, 0.0f, 0.0f };
    in.obstacleRadius = 1.0f;
    in.targetRadius = 1.5f;
    in.bossGateRadius = 2.0f;
    in.allTargetsCollected = true;
    in.landingCenter = Vec3f{ 71.0f, -0.97f, -1.0f };
    in.landingHalfX = 1.8f;
    in.landingHalfY = 1.2f;
    in.landingHalfZ = 1.5f;

    std::vector<Vec3f> obstacles;
    std::array<Vec3f, 3> targets = { Vec3f{10,0,0}, Vec3f{20,0,0}, Vec3f{30,0,0} };
    std::array<bool, 3> tCol = { true, true, true };
    std::array<Vec3f, 3> boss = { Vec3f{40.5f,0,0}, Vec3f{50,0,0}, Vec3f{60,0,0} };
    std::array<bool, 3> bPass = { false, false, false };

    auto result = collision::evaluate(in, obstacles, targets, tCol, boss, bPass);
    REQUIRE(result.bossGatesPassedThisFrame == 1);
}

TEST_CASE("collision::evaluate detects landing zone", "[collision]")
{
    collision::CollisionInput in{};
    in.rocketPos = Vec3f{ 71.0f, -0.97f, -1.0f };
    in.obstacleRadius = 1.0f;
    in.targetRadius = 1.5f;
    in.bossGateRadius = 2.0f;
    in.allTargetsCollected = true;
    in.allBossPassed = true;
    in.flightProgress = 30.0f;
    in.flightThreshold = 29.8f;
    in.landingCenter = Vec3f{ 71.0f, -0.97f, -1.0f };
    in.landingHalfX = 1.8f;
    in.landingHalfY = 1.2f;
    in.landingHalfZ = 1.5f;

    std::vector<Vec3f> obstacles;
    std::array<Vec3f, 3> targets = { Vec3f{10,0,0}, Vec3f{20,0,0}, Vec3f{30,0,0} };
    std::array<bool, 3> tCol = { true, true, true };
    std::array<Vec3f, 3> boss = { Vec3f{40,0,0}, Vec3f{50,0,0}, Vec3f{60,0,0} };
    std::array<bool, 3> bPass = { true, true, true };

    auto result = collision::evaluate(in, obstacles, targets, tCol, boss, bPass);
    REQUIRE(result.landingZoneReached);
}

TEST_CASE("collision::evaluate no landing when flight threshold not met", "[collision]")
{
    collision::CollisionInput in{};
    in.rocketPos = Vec3f{ 71.0f, -0.97f, -1.0f };
    in.obstacleRadius = 1.0f;
    in.targetRadius = 1.5f;
    in.bossGateRadius = 2.0f;
    in.allTargetsCollected = true;
    in.allBossPassed = true;
    in.flightProgress = 10.0f;
    in.flightThreshold = 29.8f;
    in.landingCenter = Vec3f{ 71.0f, -0.97f, -1.0f };
    in.landingHalfX = 1.8f;
    in.landingHalfY = 1.2f;
    in.landingHalfZ = 1.5f;

    std::vector<Vec3f> obstacles;
    std::array<Vec3f, 3> targets = { Vec3f{10,0,0}, Vec3f{20,0,0}, Vec3f{30,0,0} };
    std::array<bool, 3> tCol = { true, true, true };
    std::array<Vec3f, 3> boss = { Vec3f{40,0,0}, Vec3f{50,0,0}, Vec3f{60,0,0} };
    std::array<bool, 3> bPass = { true, true, true };

    auto result = collision::evaluate(in, obstacles, targets, tCol, boss, bPass);
    REQUIRE_FALSE(result.landingZoneReached);
}
