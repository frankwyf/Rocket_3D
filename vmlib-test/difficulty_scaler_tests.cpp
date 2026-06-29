#include <catch2/catch_amalgamated.hpp>

#include "../main/game_mechanics.hpp"

TEST_CASE("DifficultyScaler level 0 gives minimal wind", "[mechanics][difficulty]")
{
    DifficultyScaler scaler;
    scaler.setLevel(0);
    auto p = scaler.compute();
    REQUIRE(p.windBase == Catch::Approx(0.0f));
    REQUIRE(p.windGustAmp == Catch::Approx(0.0f));
    REQUIRE(p.obstacleSpeedMul == Catch::Approx(1.0f));
    REQUIRE(p.fuelDrainMul == Catch::Approx(1.0f));
}

TEST_CASE("DifficultyScaler level 5 gives max difficulty", "[mechanics][difficulty]")
{
    DifficultyScaler scaler;
    scaler.setLevel(5);
    auto p = scaler.compute();
    REQUIRE(p.windBase == Catch::Approx(1.0f));
    REQUIRE(p.windGustAmp == Catch::Approx(0.6f));
    REQUIRE(p.obstacleSpeedMul == Catch::Approx(2.5f));
    REQUIRE(p.missionTimeBonus == Catch::Approx(0.0f));
    REQUIRE(p.fuelDrainMul == Catch::Approx(1.5f));
}

TEST_CASE("DifficultyScaler intermediate level interpolates", "[mechanics][difficulty]")
{
    DifficultyScaler scaler;
    scaler.setLevel(2);
    auto p = scaler.compute();
    // t = 2/5 = 0.4
    REQUIRE(p.windBase == Catch::Approx(0.4f));
    REQUIRE(p.windGustAmp == Catch::Approx(0.24f));
    REQUIRE(p.obstacleSpeedMul == Catch::Approx(1.6f));
    REQUIRE(p.missionTimeBonus == Catch::Approx(6.0f));
    REQUIRE(p.fuelDrainMul == Catch::Approx(1.2f));
}

TEST_CASE("DifficultyScaler getLevel reports set level", "[mechanics][difficulty]")
{
    DifficultyScaler scaler;
    REQUIRE(scaler.getLevel() == 0);
    scaler.setLevel(3);
    REQUIRE(scaler.getLevel() == 3);
}

TEST_CASE("DifficultyScaler clamps at level beyond 5", "[mechanics][difficulty]")
{
    DifficultyScaler scaler;
    scaler.setLevel(10);
    auto p = scaler.compute();
    // t clamped to 1.0
    REQUIRE(p.windBase == Catch::Approx(1.0f));
    REQUIRE(p.fuelDrainMul == Catch::Approx(1.5f));
}
