#include <catch2/catch_amalgamated.hpp>

#include "../main/minimap.hpp"

using namespace minimap;

TEST_CASE("minimap::remap maps value to 0-1 range", "[minimap]")
{
    REQUIRE(remap(0.0f, 0.0f, 100.0f) == Catch::Approx(0.0f));
    REQUIRE(remap(50.0f, 0.0f, 100.0f) == Catch::Approx(0.5f));
    REQUIRE(remap(100.0f, 0.0f, 100.0f) == Catch::Approx(1.0f));
}

TEST_CASE("minimap::remap clamps out-of-range values", "[minimap]")
{
    REQUIRE(remap(-10.0f, 0.0f, 100.0f) == Catch::Approx(0.0f));
    REQUIRE(remap(200.0f, 0.0f, 100.0f) == Catch::Approx(1.0f));
}

TEST_CASE("minimap::remap handles degenerate range", "[minimap]")
{
    REQUIRE(remap(5.0f, 10.0f, 10.0f) == Catch::Approx(0.5f));
}

TEST_CASE("minimap::compute_map produces rocket point", "[minimap]")
{
    MapConfig cfg{ 0.0f, 80.0f, -10.0f, 10.0f };
    MapInput in{};
    in.rocketPos = Vec3f{ 40.0f, 5.0f, 0.0f };
    in.targets = { Vec3f{10,0,0}, Vec3f{20,0,0}, Vec3f{30,0,0} };
    in.targetCollected = { false, false, false };
    in.bossGates = { Vec3f{50,0,0}, Vec3f{60,0,0}, Vec3f{70,0,0} };
    in.bossGatePassed = { false, false, false };
    in.landingPos = Vec3f{ 71.0f, -0.97f, -1.0f };

    auto pts = compute_map(cfg, in);
    REQUIRE(pts.size() == 8); // rocket + 3 targets + 3 boss gates + landing
    REQUIRE(pts[0].kind == MapPoint::Kind::ROCKET);
    REQUIRE(pts[0].u == Catch::Approx(0.5f));
}

TEST_CASE("minimap::compute_map excludes collected targets", "[minimap]")
{
    MapConfig cfg{ 0.0f, 80.0f, -10.0f, 10.0f };
    MapInput in{};
    in.rocketPos = Vec3f{ 0.0f, 0.0f, 0.0f };
    in.targets = { Vec3f{10,0,0}, Vec3f{20,0,0}, Vec3f{30,0,0} };
    in.targetCollected = { true, true, false };
    in.bossGates = { Vec3f{50,0,0}, Vec3f{60,0,0}, Vec3f{70,0,0} };
    in.bossGatePassed = { true, true, true };
    in.landingPos = Vec3f{ 71.0f, -0.97f, -1.0f };

    auto pts = compute_map(cfg, in);
    // rocket + 1 target + 0 boss gates + landing = 3
    REQUIRE(pts.size() == 3);
}

TEST_CASE("minimap::compute_map all collected gives only rocket and landing", "[minimap]")
{
    MapConfig cfg{ 0.0f, 80.0f, -10.0f, 10.0f };
    MapInput in{};
    in.rocketPos = Vec3f{ 35.0f, 0.0f, 0.0f };
    in.targets = { Vec3f{10,0,0}, Vec3f{20,0,0}, Vec3f{30,0,0} };
    in.targetCollected = { true, true, true };
    in.bossGates = { Vec3f{50,0,0}, Vec3f{60,0,0}, Vec3f{70,0,0} };
    in.bossGatePassed = { true, true, true };
    in.landingPos = Vec3f{ 71.0f, -0.97f, -1.0f };

    auto pts = compute_map(cfg, in);
    REQUIRE(pts.size() == 2); // rocket + landing
    REQUIRE(pts[0].kind == MapPoint::Kind::ROCKET);
    REQUIRE(pts[1].kind == MapPoint::Kind::LANDING);
}
