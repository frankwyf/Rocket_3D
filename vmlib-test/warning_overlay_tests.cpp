#include <catch2/catch_amalgamated.hpp>

#include "../main/warning_overlay.hpp"

using namespace warning_overlay;

TEST_CASE("classify_fuel returns NONE when fuel is high", "[warning]")
{
    REQUIRE(classify_fuel(0.80f) == WarningLevel::NONE);
    REQUIRE(classify_fuel(0.50f) == WarningLevel::NONE);
    REQUIRE(classify_fuel(0.26f) == WarningLevel::NONE);
}

TEST_CASE("classify_fuel returns CAUTION at 25%", "[warning]")
{
    REQUIRE(classify_fuel(0.25f) == WarningLevel::CAUTION);
    REQUIRE(classify_fuel(0.15f) == WarningLevel::CAUTION);
    REQUIRE(classify_fuel(0.11f) == WarningLevel::CAUTION);
}

TEST_CASE("classify_fuel returns CRITICAL at 10%", "[warning]")
{
    REQUIRE(classify_fuel(0.10f) == WarningLevel::CRITICAL);
    REQUIRE(classify_fuel(0.05f) == WarningLevel::CRITICAL);
    REQUIRE(classify_fuel(0.0f) == WarningLevel::CRITICAL);
}

TEST_CASE("classify_timer returns NONE when time is ample", "[warning]")
{
    REQUIRE(classify_timer(30.0f) == WarningLevel::NONE);
    REQUIRE(classify_timer(16.0f) == WarningLevel::NONE);
}

TEST_CASE("classify_timer returns CAUTION under 15s", "[warning]")
{
    REQUIRE(classify_timer(15.0f) == WarningLevel::CAUTION);
    REQUIRE(classify_timer(10.0f) == WarningLevel::CAUTION);
    REQUIRE(classify_timer(6.0f) == WarningLevel::CAUTION);
}

TEST_CASE("classify_timer returns CRITICAL under 5s", "[warning]")
{
    REQUIRE(classify_timer(5.0f) == WarningLevel::CRITICAL);
    REQUIRE(classify_timer(2.0f) == WarningLevel::CRITICAL);
    REQUIRE(classify_timer(0.0f) == WarningLevel::CRITICAL);
}

TEST_CASE("compute_flash returns 0 for NONE", "[warning]")
{
    REQUIRE(compute_flash(1.0f, WarningLevel::NONE) == Catch::Approx(0.0f));
}

TEST_CASE("compute_flash returns positive for CAUTION", "[warning]")
{
    float v = compute_flash(0.0f, WarningLevel::CAUTION);
    REQUIRE(v >= 0.0f);
    REQUIRE(v <= 1.0f);
}

TEST_CASE("compute_flash varies over time for CRITICAL", "[warning]")
{
    float a = compute_flash(0.0f, WarningLevel::CRITICAL);
    float b = compute_flash(0.5f, WarningLevel::CRITICAL);
    // They should differ due to high frequency
    bool differs = (a != b);
    REQUIRE(differs);
}

TEST_CASE("evaluate combines fuel and timer warnings", "[warning]")
{
    auto ws = evaluate(0.05f, 3.0f, 1.0f);
    REQUIRE(ws.fuelWarning == WarningLevel::CRITICAL);
    REQUIRE(ws.timerWarning == WarningLevel::CRITICAL);
    REQUIRE(ws.flashIntensity > 0.0f);
}

TEST_CASE("evaluate returns NONE when everything is fine", "[warning]")
{
    auto ws = evaluate(0.80f, 30.0f, 0.0f);
    REQUIRE(ws.fuelWarning == WarningLevel::NONE);
    REQUIRE(ws.timerWarning == WarningLevel::NONE);
    REQUIRE(ws.flashIntensity == Catch::Approx(0.0f));
}
