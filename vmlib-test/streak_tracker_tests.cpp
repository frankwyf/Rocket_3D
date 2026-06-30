#include <catch2/catch_amalgamated.hpp>

#include "../main/game_mechanics.hpp"

TEST_CASE("StreakTracker starts at zero", "[mechanics][streak]")
{
    StreakTracker tracker;
    REQUIRE(tracker.getCurrent() == 0);
    REQUIRE(tracker.getBest() == 0);
    REQUIRE(tracker.getMultiplier() == Catch::Approx(1.0f));
}

TEST_CASE("StreakTracker increments on success", "[mechanics][streak]")
{
    StreakTracker tracker;
    tracker.recordSuccess();
    REQUIRE(tracker.getCurrent() == 1);
    REQUIRE(tracker.getMultiplier() == Catch::Approx(1.2f));
    tracker.recordSuccess();
    REQUIRE(tracker.getCurrent() == 2);
    REQUIRE(tracker.getMultiplier() == Catch::Approx(1.2f));
    tracker.recordSuccess();
    REQUIRE(tracker.getCurrent() == 3);
    REQUIRE(tracker.getMultiplier() == Catch::Approx(1.5f));
}

TEST_CASE("StreakTracker resets on failure", "[mechanics][streak]")
{
    StreakTracker tracker;
    tracker.recordSuccess();
    tracker.recordSuccess();
    tracker.recordSuccess();
    REQUIRE(tracker.getCurrent() == 3);

    tracker.recordFailure();
    REQUIRE(tracker.getCurrent() == 0);
    REQUIRE(tracker.getMultiplier() == Catch::Approx(1.0f));
}

TEST_CASE("StreakTracker tracks best streak", "[mechanics][streak]")
{
    StreakTracker tracker;
    tracker.recordSuccess();
    tracker.recordSuccess();
    tracker.recordSuccess();
    tracker.recordSuccess();
    REQUIRE(tracker.getBest() == 4);

    tracker.recordFailure();
    REQUIRE(tracker.getBest() == 4);
    REQUIRE(tracker.getCurrent() == 0);

    tracker.recordSuccess();
    tracker.recordSuccess();
    REQUIRE(tracker.getBest() == 4);
}

TEST_CASE("StreakTracker multiplier reaches 2x at 5 streak", "[mechanics][streak]")
{
    StreakTracker tracker;
    for (int i = 0; i < 5; ++i)
        tracker.recordSuccess();
    REQUIRE(tracker.getMultiplier() == Catch::Approx(2.0f));
}

TEST_CASE("StreakTracker reset clears everything", "[mechanics][streak]")
{
    StreakTracker tracker;
    tracker.recordSuccess();
    tracker.recordSuccess();
    tracker.recordSuccess();
    tracker.reset();
    REQUIRE(tracker.getCurrent() == 0);
    REQUIRE(tracker.getBest() == 0);
}
