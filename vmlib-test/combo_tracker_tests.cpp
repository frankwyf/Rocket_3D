#include <catch2/catch_amalgamated.hpp>

#include "../main/game_mechanics.hpp"

TEST_CASE("ComboTracker awards escalating bonus for rapid collects", "[mechanics][combo]")
{
    ComboTracker tracker(3.0f, 50.0f);

    int b1 = tracker.collect(1.0f);
    REQUIRE(b1 == 50);

    int b2 = tracker.collect(2.0f);
    REQUIRE(b2 == 100);

    int b3 = tracker.collect(3.5f);
    REQUIRE(b3 == 150);

    REQUIRE(tracker.getComboCount() == 3);
    REQUIRE(tracker.getTotalBonus() == 300);
}

TEST_CASE("ComboTracker resets combo after window expires", "[mechanics][combo]")
{
    ComboTracker tracker(2.0f, 50.0f);

    tracker.collect(0.0f);
    tracker.collect(1.0f);
    REQUIRE(tracker.getComboCount() == 2);

    int b = tracker.collect(5.0f);
    REQUIRE(tracker.getComboCount() == 1);
    REQUIRE(b == 50);
}

TEST_CASE("ComboTracker update detects window expiry", "[mechanics][combo]")
{
    ComboTracker tracker(2.0f, 50.0f);
    tracker.collect(1.0f);
    REQUIRE(tracker.isComboActive(2.0f));

    tracker.update(4.0f);
    REQUIRE(tracker.getComboCount() == 0);
    REQUIRE_FALSE(tracker.isComboActive(4.0f));
}

TEST_CASE("ComboTracker reset clears all state", "[mechanics][combo]")
{
    ComboTracker tracker(3.0f, 50.0f);
    tracker.collect(0.0f);
    tracker.collect(1.0f);
    REQUIRE(tracker.getComboCount() == 2);

    tracker.reset();
    REQUIRE(tracker.getComboCount() == 0);
    REQUIRE(tracker.getTotalBonus() == 0);
}

TEST_CASE("ComboTracker getComboWindow returns configured window", "[mechanics][combo]")
{
    ComboTracker tracker(4.5f, 100.0f);
    REQUIRE(tracker.getComboWindow() == Catch::Approx(4.5f));
}
