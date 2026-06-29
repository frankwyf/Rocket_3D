#include <catch2/catch_amalgamated.hpp>

#include "../main/game_mechanics.hpp"

TEST_CASE("AchievementTracker starts with nothing unlocked", "[mechanics][achievement]")
{
    AchievementTracker tracker;
    REQUIRE(tracker.getUnlockedCount() == 0);
    REQUIRE(tracker.getTotalCount() == static_cast<int>(AchievementId::COUNT));
}

TEST_CASE("AchievementTracker unlock returns true on first unlock", "[mechanics][achievement]")
{
    AchievementTracker tracker;
    REQUIRE(tracker.unlock(AchievementId::FIRST_LAUNCH));
    REQUIRE(tracker.isUnlocked(AchievementId::FIRST_LAUNCH));
    REQUIRE(tracker.getUnlockedCount() == 1);
}

TEST_CASE("AchievementTracker unlock returns false on duplicate", "[mechanics][achievement]")
{
    AchievementTracker tracker;
    tracker.unlock(AchievementId::FIRST_LANDING);
    REQUIRE_FALSE(tracker.unlock(AchievementId::FIRST_LANDING));
    REQUIRE(tracker.getUnlockedCount() == 1);
}

TEST_CASE("AchievementTracker tracks multiple distinct unlocks", "[mechanics][achievement]")
{
    AchievementTracker tracker;
    tracker.unlock(AchievementId::FIRST_LAUNCH);
    tracker.unlock(AchievementId::COMBO_3);
    tracker.unlock(AchievementId::SPEED_DEMON);
    REQUIRE(tracker.getUnlockedCount() == 3);
    REQUIRE(tracker.isUnlocked(AchievementId::FIRST_LAUNCH));
    REQUIRE(tracker.isUnlocked(AchievementId::COMBO_3));
    REQUIRE(tracker.isUnlocked(AchievementId::SPEED_DEMON));
    REQUIRE_FALSE(tracker.isUnlocked(AchievementId::CAMPAIGN_COMPLETE));
}

TEST_CASE("AchievementTracker reset clears all unlocks", "[mechanics][achievement]")
{
    AchievementTracker tracker;
    tracker.unlock(AchievementId::FIRST_LAUNCH);
    tracker.unlock(AchievementId::FIRST_LANDING);
    tracker.unlock(AchievementId::CAMPAIGN_COMPLETE);
    REQUIRE(tracker.getUnlockedCount() == 3);

    tracker.reset();
    REQUIRE(tracker.getUnlockedCount() == 0);
    REQUIRE_FALSE(tracker.isUnlocked(AchievementId::FIRST_LAUNCH));
}

TEST_CASE("AchievementTracker getTotalCount matches enum COUNT", "[mechanics][achievement]")
{
    AchievementTracker tracker;
    REQUIRE(tracker.getTotalCount() == static_cast<int>(AchievementId::COUNT));
    // Unlock all and verify
    for (int i = 0; i < static_cast<int>(AchievementId::COUNT); ++i)
        tracker.unlock(static_cast<AchievementId>(i));
    REQUIRE(tracker.getUnlockedCount() == tracker.getTotalCount());
}

TEST_CASE("AchievementTracker invalid id does not crash", "[mechanics][achievement]")
{
    AchievementTracker tracker;
    REQUIRE_FALSE(tracker.unlock(AchievementId::COUNT));
    REQUIRE_FALSE(tracker.isUnlocked(AchievementId::COUNT));
}
