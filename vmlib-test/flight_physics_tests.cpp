#include <catch2/catch_amalgamated.hpp>

#include "../main/flight_physics.hpp"

using namespace flight_physics;

TEST_CASE("compute_trajectory at time 0 gives origin", "[physics]")
{
    FlightState fs{ 0.0f, 0.0f, 0.0f };
    auto out = compute_trajectory(fs);
    REQUIRE(out.remoteX == Catch::Approx(0.0f).margin(0.001f));
    REQUIRE(out.remoteY == Catch::Approx(0.0f).margin(0.001f));
    REQUIRE(out.remoteZ == Catch::Approx(0.0f));
}

TEST_CASE("compute_trajectory advances with time", "[physics]")
{
    FlightState fs{ 10.0f, 0.0f, 0.0f };
    auto out = compute_trajectory(fs);
    REQUIRE(out.remoteX > 0.0f);
    REQUIRE(out.remoteY > 0.0f);
}

TEST_CASE("compute_trajectory includes offsetY and offsetZ", "[physics]")
{
    FlightState fs{ 10.0f, 2.0f, -1.5f };
    FlightState fs0{ 10.0f, 0.0f, 0.0f };
    auto out = compute_trajectory(fs);
    auto out0 = compute_trajectory(fs0);
    REQUIRE(out.remoteY == Catch::Approx(out0.remoteY + 2.0f));
    REQUIRE(out.remoteZ == Catch::Approx(-1.5f));
}

TEST_CASE("compute_steer no input gives zero", "[physics]")
{
    SteerInput si{ false, false, false, false, 5.0f, 0.7f, 0.1f };
    auto r = compute_steer(si);
    REQUIRE(r.deltaZ == Catch::Approx(0.0f));
    REQUIRE(r.deltaY == Catch::Approx(0.0f));
    REQUIRE(r.fuelCost == Catch::Approx(0.0f));
    REQUIRE_FALSE(r.steered);
}

TEST_CASE("compute_steer left moves Z negative", "[physics]")
{
    SteerInput si{ true, false, false, false, 5.0f, 0.7f, 0.5f };
    auto r = compute_steer(si);
    REQUIRE(r.deltaZ == Catch::Approx(-2.5f));
    REQUIRE(r.fuelCost > 0.0f);
    REQUIRE(r.steered);
}

TEST_CASE("compute_steer up moves Y positive", "[physics]")
{
    SteerInput si{ false, false, true, false, 5.0f, 0.7f, 1.0f };
    auto r = compute_steer(si);
    REQUIRE(r.deltaY == Catch::Approx(3.5f));
    REQUIRE(r.steered);
}

TEST_CASE("clamp_offsets enforces limits", "[physics]")
{
    float y = 10.0f;
    float z = -10.0f;
    ClampLimits lim{ -6.0f, 6.0f, -2.5f, 4.0f };
    clamp_offsets(y, z, lim);
    REQUIRE(y == Catch::Approx(4.0f));
    REQUIRE(z == Catch::Approx(-6.0f));
}

TEST_CASE("clamp_offsets does nothing inside range", "[physics]")
{
    float y = 1.0f;
    float z = 2.0f;
    ClampLimits lim{ -6.0f, 6.0f, -2.5f, 4.0f };
    clamp_offsets(y, z, lim);
    REQUIRE(y == Catch::Approx(1.0f));
    REQUIRE(z == Catch::Approx(2.0f));
}

TEST_CASE("advance_shoot_time increments by speed", "[physics]")
{
    float t = advance_shoot_time(5.0f, 0.5f, 1.0f);
    REQUIRE(t == Catch::Approx(8.0f));
}

TEST_CASE("advance_shoot_time clamps at max", "[physics]")
{
    float t = advance_shoot_time(29.0f, 1.0f, 2.0f);
    REQUIRE(t == Catch::Approx(30.0f));
}
