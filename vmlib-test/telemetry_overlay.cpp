#include <catch2/catch_amalgamated.hpp>

#include "../main/telemetry_overlay.hpp"

TEST_CASE("Telemetry history keeps fixed-capacity recent samples", "[telemetry][history]")
{
    telemetry_overlay::TelemetryHistory history(3);

    history.push({ 0.0f, 1.0f, 3.0f, 90.0f });
    history.push({ 1.0f, 2.0f, 4.0f, 80.0f });
    history.push({ 2.0f, 3.0f, 5.0f, 70.0f });
    history.push({ 3.0f, 4.0f, 6.0f, 60.0f });

    REQUIRE(history.size() == 3);
    REQUIRE(history.data().front().timeS == Catch::Approx(1.0f));
    REQUIRE(history.data().back().timeS == Catch::Approx(3.0f));
}

TEST_CASE("Risk score is clamped to [0,1] and reacts to dangerous states", "[telemetry][risk]")
{
    telemetry_overlay::TelemetrySample safe{ 0.0f, 12.0f, 4.0f, 95.0f };
    telemetry_overlay::TelemetrySample risky{ 0.0f, 0.5f, 28.0f, 8.0f };

    float safeScore = telemetry_overlay::compute_risk_score(safe);
    float riskyScore = telemetry_overlay::compute_risk_score(risky);

    REQUIRE(safeScore >= 0.0f);
    REQUIRE(riskyScore <= 1.0f);
    REQUIRE(riskyScore > safeScore);
}

TEST_CASE("Polyline generation produces monotonic X and bounded Y", "[telemetry][overlay]")
{
    telemetry_overlay::TelemetryHistory history(8);
    history.push({ 0.0f, 2.0f, 2.0f, 100.0f });
    history.push({ 1.0f, 4.0f, 4.0f, 95.0f });
    history.push({ 2.0f, 3.0f, 6.0f, 90.0f });
    history.push({ 3.0f, 5.0f, 8.0f, 85.0f });

    auto points = telemetry_overlay::build_metric_polyline(
        history,
        telemetry_overlay::Metric::Altitude,
        -0.9f,
        0.8f,
        0.5f,
        0.3f
    );

    REQUIRE(points.size() == history.size());

    for (std::size_t i = 1; i < points.size(); ++i)
    {
        REQUIRE(points[i].x > points[i - 1].x);
    }

    for (const auto& p : points)
    {
        REQUIRE(p.y <= Catch::Approx(0.8f).margin(0.0001f));
        REQUIRE(p.y >= Catch::Approx(0.5f).margin(0.0001f));
    }
}

TEST_CASE("Risk color shifts from greenish to reddish", "[telemetry][color]")
{
    auto low = telemetry_overlay::risk_to_color(0.0f);
    auto high = telemetry_overlay::risk_to_color(1.0f);

    REQUIRE(high.r > low.r);
    REQUIRE(high.g < low.g);
}
