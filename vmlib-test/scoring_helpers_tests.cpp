#include <catch2/catch_amalgamated.hpp>

#include "../main/scoring_helpers.hpp"

TEST_CASE("compute_mission_score base is always 500", "[scoring]")
{
    auto bonus = scoring::compute_mission_score(0.0f, 0.0f, 0);
    REQUIRE(bonus.baseScore == 500);
    REQUIRE(bonus.total == 500);
}

TEST_CASE("compute_mission_score includes time bonus", "[scoring]")
{
    auto bonus = scoring::compute_mission_score(10.0f, 0.0f, 0);
    REQUIRE(bonus.timeBonus == 100);
    REQUIRE(bonus.total == 600);
}

TEST_CASE("compute_mission_score includes fuel bonus", "[scoring]")
{
    auto bonus = scoring::compute_mission_score(0.0f, 20.0f, 0);
    REQUIRE(bonus.fuelBonus == 100);
    REQUIRE(bonus.total == 600);
}

TEST_CASE("compute_mission_score includes combo bonus", "[scoring]")
{
    auto bonus = scoring::compute_mission_score(0.0f, 0.0f, 250);
    REQUIRE(bonus.comboBonus == 250);
    REQUIRE(bonus.total == 750);
}

TEST_CASE("compute_mission_score combines all components", "[scoring]")
{
    auto bonus = scoring::compute_mission_score(5.0f, 10.0f, 100);
    REQUIRE(bonus.baseScore == 500);
    REQUIRE(bonus.timeBonus == 50);
    REQUIRE(bonus.fuelBonus == 50);
    REQUIRE(bonus.comboBonus == 100);
    REQUIRE(bonus.total == 700);
}

TEST_CASE("compute_target_score returns 100 plus combo", "[scoring]")
{
    REQUIRE(scoring::compute_target_score(0) == 100);
    REQUIRE(scoring::compute_target_score(50) == 150);
    REQUIRE(scoring::compute_target_score(200) == 300);
}

TEST_CASE("compute_boss_gate_score always 150", "[scoring]")
{
    REQUIRE(scoring::compute_boss_gate_score() == 150);
}

TEST_CASE("rate_launch S rating for 90%+", "[scoring]")
{
    auto r = scoring::rate_launch(900, 1000);
    REQUIRE(std::string(r.label) == "S");
    REQUIRE(r.fraction == Catch::Approx(0.9f));
}

TEST_CASE("rate_launch D rating for low score", "[scoring]")
{
    auto r = scoring::rate_launch(100, 1000);
    REQUIRE(std::string(r.label) == "D");
}

TEST_CASE("rate_launch handles zero max", "[scoring]")
{
    auto r = scoring::rate_launch(100, 0);
    REQUIRE(std::string(r.label) == "---");
    REQUIRE(r.fraction == Catch::Approx(0.0f));
}

TEST_CASE("rate_launch B rating for middle score", "[scoring]")
{
    auto r = scoring::rate_launch(600, 1000);
    REQUIRE(std::string(r.label) == "B");
}
