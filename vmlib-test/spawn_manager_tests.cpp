#include <catch2/catch_amalgamated.hpp>

#include "../main/spawn_manager.hpp"

using namespace spawn_manager;

TEST_CASE("config_for_level 0 has base obstacle count", "[spawn]")
{
    auto cfg = config_for_level(0);
    REQUIRE(cfg.obstacleCount == 3);
    REQUIRE(cfg.fuelCanisterCount == 2);
}

TEST_CASE("config_for_level increases obstacles with level", "[spawn]")
{
    auto cfg0 = config_for_level(0);
    auto cfg3 = config_for_level(3);
    auto cfg5 = config_for_level(5);
    REQUIRE(cfg3.obstacleCount > cfg0.obstacleCount);
    REQUIRE(cfg5.obstacleCount > cfg3.obstacleCount);
}

TEST_CASE("config_for_level hard levels get 3 canisters", "[spawn]")
{
    REQUIRE(config_for_level(3).fuelCanisterCount == 3);
    REQUIRE(config_for_level(5).fuelCanisterCount == 3);
}

TEST_CASE("generate_obstacle_positions produces correct count", "[spawn]")
{
    auto cfg = config_for_level(2);
    auto pos = generate_obstacle_positions(cfg, 42);
    REQUIRE(pos.size() == static_cast<std::size_t>(cfg.obstacleCount));
}

TEST_CASE("generate_obstacle_positions all within path bounds", "[spawn]")
{
    auto cfg = config_for_level(4);
    auto pos = generate_obstacle_positions(cfg, 99);
    for (auto const& p : pos)
    {
        REQUIRE(p.x >= cfg.pathStartX);
        REQUIRE(p.x <= cfg.pathEndX);
    }
}

TEST_CASE("generate_obstacle_positions different seeds give different results", "[spawn]")
{
    auto cfg = config_for_level(3);
    auto pos1 = generate_obstacle_positions(cfg, 1);
    auto pos2 = generate_obstacle_positions(cfg, 2);
    bool anyDiff = false;
    for (std::size_t i = 0; i < pos1.size(); ++i)
    {
        if (pos1[i].z != pos2[i].z)
        {
            anyDiff = true;
            break;
        }
    }
    REQUIRE(anyDiff);
}

TEST_CASE("generate_fuel_positions produces correct count", "[spawn]")
{
    auto cfg = config_for_level(1);
    auto pos = generate_fuel_positions(cfg, 7);
    REQUIRE(pos.size() == static_cast<std::size_t>(cfg.fuelCanisterCount));
}

TEST_CASE("generate_fuel_positions within path range", "[spawn]")
{
    auto cfg = config_for_level(5);
    auto pos = generate_fuel_positions(cfg, 123);
    for (auto const& p : pos)
    {
        REQUIRE(p.x >= cfg.pathStartX);
        REQUIRE(p.x <= cfg.pathEndX);
    }
}
