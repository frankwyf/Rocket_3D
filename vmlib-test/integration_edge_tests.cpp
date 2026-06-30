#include <catch2/catch_amalgamated.hpp>

#include "../main/game_mechanics.hpp"
#include "../main/collision_helpers.hpp"
#include "../main/flight_physics.hpp"
#include "../main/scoring_helpers.hpp"
#include "../main/warning_overlay.hpp"
#include "../main/hud_renderer.hpp"

// ComboTracker edge cases
TEST_CASE("ComboTracker collect at exact window boundary", "[integration][edge]")
{
    ComboTracker tracker(2.0f, 50.0f);
    tracker.collect(0.0f);
    // Collect exactly at window edge (time diff == window)
    int b = tracker.collect(2.0f);
    // Should still count as combo since !(diff > window)
    REQUIRE(tracker.getComboCount() == 2);
    REQUIRE(b == 100);
}

// FuelPickupManager edge: zero radius
TEST_CASE("FuelPickupManager zero pickup radius collects nothing", "[integration][edge]")
{
    FuelPickupManager mgr;
    mgr.addCanister(FuelCanister(Vec3f{ 0.0f, 0.0f, 0.0f }, 10.0f));
    float r = mgr.checkCollection(Vec3f{ 0.0f, 0.0f, 0.0f }, 0.0f);
    REQUIRE(r == Catch::Approx(0.0f));
}

// WindSystem zero frequency
TEST_CASE("WindSystem zero frequency and base returns base only", "[integration][edge]")
{
    WindSystem wind(5.0f, 0.0f, 2.0f);
    // With freq=0, sin(0) = 0 for any time
    wind.update(100.0f);
    REQUIRE(wind.getCurrentOffset() == Catch::Approx(5.0f));
}

// DifficultyScaler negative level
TEST_CASE("DifficultyScaler negative level treated as minimal", "[integration][edge]")
{
    DifficultyScaler scaler;
    scaler.setLevel(-1);
    auto p = scaler.compute();
    // t = -1/5 = -0.2, but we only clamp >1, so it can go negative
    // This is fine - the params will be negative/zero which is mild
    REQUIRE(p.windBase <= 0.0f);
}

// collision::evaluate with empty obstacles
TEST_CASE("collision::evaluate no obstacles means no hit", "[integration][edge]")
{
    collision::CollisionInput in{};
    in.rocketPos = Vec3f{ 0.0f, 0.0f, 0.0f };
    in.obstacleRadius = 100.0f;
    in.targetRadius = 1.0f;
    in.bossGateRadius = 1.0f;
    in.landingCenter = Vec3f{ 71.0f, -0.97f, -1.0f };
    in.landingHalfX = 1.8f;
    in.landingHalfY = 1.2f;
    in.landingHalfZ = 1.5f;

    std::vector<Vec3f> obstacles; // empty
    std::array<Vec3f, 3> targets = { Vec3f{10,0,0}, Vec3f{20,0,0}, Vec3f{30,0,0} };
    std::array<bool, 3> tCol = { false, false, false };
    std::array<Vec3f, 3> boss = { Vec3f{40,0,0}, Vec3f{50,0,0}, Vec3f{60,0,0} };
    std::array<bool, 3> bPass = { false, false, false };

    auto result = collision::evaluate(in, obstacles, targets, tCol, boss, bPass);
    REQUIRE_FALSE(result.hitObstacle);
}

// flight_physics: max shootTime does not exceed limit
TEST_CASE("flight_physics advance from max stays at max", "[integration][edge]")
{
    float t = flight_physics::advance_shoot_time(30.0f, 1.0f, 2.0f);
    REQUIRE(t == Catch::Approx(30.0f));
}

// scoring: very large timer and fuel
TEST_CASE("scoring compute handles large values", "[integration][edge]")
{
    auto bonus = scoring::compute_mission_score(999.0f, 999.0f, 5000);
    REQUIRE(bonus.total > 5000);
    REQUIRE(bonus.timeBonus == 9990);
    REQUIRE(bonus.fuelBonus == 4995);
}

// warning_overlay: fuel fraction exactly at boundary
TEST_CASE("warning_overlay boundary at 0.25 is CAUTION", "[integration][edge]")
{
    REQUIRE(warning_overlay::classify_fuel(0.25f) == warning_overlay::WarningLevel::CAUTION);
    REQUIRE(warning_overlay::classify_timer(15.0f) == warning_overlay::WarningLevel::CAUTION);
}

// hud_renderer: zero maxFuel does not divide by zero
TEST_CASE("hud_renderer zero maxFuel gives zero fraction", "[integration][edge]")
{
    hud_renderer::HudInput hi{};
    hi.levelName = "X";
    hi.camPresetName = "Y";
    hi.maxFuel = 0.0f;
    hi.fuel = 50.0f;
    hi.totalLevels = 1;
    auto hud = hud_renderer::compute_hud(hi);
    REQUIRE(hud.fuelFraction == Catch::Approx(0.0f));
}

// StreakTracker multiple failures don't go negative
TEST_CASE("StreakTracker multiple failures stay at zero", "[integration][edge]")
{
    StreakTracker tracker;
    tracker.recordFailure();
    tracker.recordFailure();
    tracker.recordFailure();
    REQUIRE(tracker.getCurrent() == 0);
    REQUIRE(tracker.getBest() == 0);
}

// AchievementTracker unlocking all and checking count
TEST_CASE("AchievementTracker unlock all gives full count", "[integration][edge]")
{
    AchievementTracker tracker;
    for (int i = 0; i < static_cast<int>(AchievementId::COUNT); ++i)
        tracker.unlock(static_cast<AchievementId>(i));
    REQUIRE(tracker.getUnlockedCount() == tracker.getTotalCount());
}
