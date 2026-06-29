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

TEST_CASE("Telemetry averages and trend detection reflect recent motion", "[telemetry][analytics]")
{
    telemetry_overlay::TelemetryHistory history(10);
    history.push({ 0.0f, 1.0f, 4.0f, 100.0f });
    history.push({ 1.0f, 2.0f, 5.0f, 95.0f });
    history.push({ 2.0f, 3.0f, 6.0f, 90.0f });
    history.push({ 3.0f, 4.0f, 7.0f, 85.0f });

    float avgAllAltitude = telemetry_overlay::compute_metric_average(history, telemetry_overlay::Metric::Altitude);
    float avgTailFuel = telemetry_overlay::compute_metric_average(history, telemetry_overlay::Metric::Fuel, 2);

    REQUIRE(avgAllAltitude == Catch::Approx(2.5f));
    REQUIRE(avgTailFuel == Catch::Approx(87.5f));

    auto speedTrend = telemetry_overlay::detect_metric_trend(history, telemetry_overlay::Metric::Speed, 4);
    auto fuelTrend = telemetry_overlay::detect_metric_trend(history, telemetry_overlay::Metric::Fuel, 4);

    REQUIRE(speedTrend == telemetry_overlay::Trend::Up);
    REQUIRE(fuelTrend == telemetry_overlay::Trend::Down);
}

TEST_CASE("Radar marker builds symmetric cross points", "[telemetry][overlay]")
{
    auto marker = telemetry_overlay::make_radar_marker(0.2f, -0.3f, 0.5f);
    REQUIRE(marker[0].x == Catch::Approx(0.2f));
    REQUIRE(marker[0].y == Catch::Approx(0.2f));
    REQUIRE(marker[2].x == Catch::Approx(0.2f));
    REQUIRE(marker[2].y == Catch::Approx(-0.8f));
    REQUIRE(marker[1].x == Catch::Approx(0.7f));
    REQUIRE(marker[3].x == Catch::Approx(-0.3f));
}

TEST_CASE("Trend detection reports stable for near-constant values", "[telemetry][analytics]")
{
    telemetry_overlay::TelemetryHistory history(6);
    history.push({ 0.0f, 5.0000f, 3.0f, 60.0f });
    history.push({ 1.0f, 5.0004f, 3.0f, 60.0f });
    history.push({ 2.0f, 5.0002f, 3.0f, 60.0f });

    auto trend = telemetry_overlay::detect_metric_trend(history, telemetry_overlay::Metric::Altitude, 3, 0.001f);
    REQUIRE(trend == telemetry_overlay::Trend::Stable);
}

TEST_CASE("Average uses entire history when tail count exceeds size", "[telemetry][analytics]")
{
    telemetry_overlay::TelemetryHistory history(4);
    history.push({ 0.0f, 1.0f, 2.0f, 50.0f });
    history.push({ 1.0f, 3.0f, 4.0f, 70.0f });

    float avgSpeed = telemetry_overlay::compute_metric_average(history, telemetry_overlay::Metric::Speed, 99);
    REQUIRE(avgSpeed == Catch::Approx(3.0f));
}
