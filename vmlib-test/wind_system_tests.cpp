#include <catch2/catch_amalgamated.hpp>

#include "../main/game_mechanics.hpp"

TEST_CASE("WindSystem default has zero offset", "[mechanics][wind]")
{
    WindSystem wind(0.0f, 1.0f, 0.0f);
    wind.update(0.0f);
    REQUIRE(wind.getCurrentOffset() == Catch::Approx(0.0f));
}

TEST_CASE("WindSystem with zero amplitude only returns base", "[mechanics][wind]")
{
    WindSystem wind(2.5f, 1.0f, 0.0f);
    wind.update(1.0f);
    REQUIRE(wind.getCurrentOffset() == Catch::Approx(2.5f));
    wind.update(3.7f);
    REQUIRE(wind.getCurrentOffset() == Catch::Approx(2.5f));
}

TEST_CASE("WindSystem computeOffset matches update result", "[mechanics][wind]")
{
    WindSystem wind(1.0f, 2.0f, 0.5f);
    float expected = wind.computeOffset(4.0f);
    wind.update(4.0f);
    REQUIRE(wind.getCurrentOffset() == Catch::Approx(expected));
}

TEST_CASE("WindSystem varies over time with nonzero amplitude", "[mechanics][wind]")
{
    WindSystem wind(0.0f, 1.0f, 1.0f);
    wind.update(0.0f);
    float a = wind.getCurrentOffset();
    wind.update(1.0f);
    float b = wind.getCurrentOffset();
    wind.update(2.5f);
    float c = wind.getCurrentOffset();
    // Not all values can be identical given sine-based variation
    bool varied = (a != b) || (b != c);
    REQUIRE(varied);
}

TEST_CASE("WindSystem setParameters changes behavior", "[mechanics][wind]")
{
    WindSystem wind(0.0f, 1.0f, 0.0f);
    wind.update(1.0f);
    REQUIRE(wind.getCurrentOffset() == Catch::Approx(0.0f));

    wind.setParameters(3.0f, 2.0f, 1.0f);
    wind.update(1.0f);
    REQUIRE(wind.getCurrentOffset() != Catch::Approx(0.0f));
}

TEST_CASE("WindSystem getters reflect construction parameters", "[mechanics][wind]")
{
    WindSystem wind(1.5f, 2.5f, 0.8f);
    REQUIRE(wind.getBaseStrength() == Catch::Approx(1.5f));
    REQUIRE(wind.getGustFrequency() == Catch::Approx(2.5f));
    REQUIRE(wind.getGustAmplitude() == Catch::Approx(0.8f));
}

TEST_CASE("WindSystem bounded offset for gameplay-realistic params", "[mechanics][wind]")
{
    WindSystem wind(0.8f, 1.5f, 0.4f);
    float maxSeen = 0.0f;
    for (int i = 0; i < 100; ++i)
    {
        wind.update(static_cast<float>(i) * 0.1f);
        float abs = wind.getCurrentOffset() < 0 ? -wind.getCurrentOffset() : wind.getCurrentOffset();
        if (abs > maxSeen) maxSeen = abs;
    }
    // base(0.8) + gust amplitude sum ~= 0.8 + 0.4 + 0.2 = 1.4 max
    REQUIRE(maxSeen < 2.0f);
}
