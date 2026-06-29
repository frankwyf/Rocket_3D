#include <catch2/catch_amalgamated.hpp>

#include "../main/game_mechanics.hpp"

TEST_CASE("FuelPickupManager collects canister within radius", "[mechanics][fuel]")
{
    FuelPickupManager mgr;
    mgr.addCanister(FuelCanister(Vec3f{ 10.0f, 5.0f, -1.0f }, 25.0f));

    float restored = mgr.checkCollection(Vec3f{ 10.5f, 5.2f, -1.1f }, 2.0f);
    REQUIRE(restored == Catch::Approx(25.0f));
    REQUIRE(mgr.getRemainingCount() == 0);
}

TEST_CASE("FuelPickupManager ignores canister outside radius", "[mechanics][fuel]")
{
    FuelPickupManager mgr;
    mgr.addCanister(FuelCanister(Vec3f{ 10.0f, 5.0f, -1.0f }, 25.0f));

    float restored = mgr.checkCollection(Vec3f{ 50.0f, 50.0f, 50.0f }, 2.0f);
    REQUIRE(restored == Catch::Approx(0.0f));
    REQUIRE(mgr.getRemainingCount() == 1);
}

TEST_CASE("FuelPickupManager does not double-collect", "[mechanics][fuel]")
{
    FuelPickupManager mgr;
    mgr.addCanister(FuelCanister(Vec3f{ 5.0f, 5.0f, 0.0f }, 20.0f));

    mgr.checkCollection(Vec3f{ 5.0f, 5.0f, 0.0f }, 1.0f);
    float second = mgr.checkCollection(Vec3f{ 5.0f, 5.0f, 0.0f }, 1.0f);
    REQUIRE(second == Catch::Approx(0.0f));
}

TEST_CASE("FuelPickupManager resetAll restores all canisters", "[mechanics][fuel]")
{
    FuelPickupManager mgr;
    mgr.addCanister(FuelCanister(Vec3f{ 1.0f, 1.0f, 0.0f }, 10.0f));
    mgr.addCanister(FuelCanister(Vec3f{ 2.0f, 2.0f, 0.0f }, 15.0f));

    mgr.checkCollection(Vec3f{ 1.0f, 1.0f, 0.0f }, 1.0f);
    REQUIRE(mgr.getRemainingCount() == 1);

    mgr.resetAll();
    REQUIRE(mgr.getRemainingCount() == 2);
}

TEST_CASE("FuelPickupManager update advances rotation", "[mechanics][fuel]")
{
    FuelPickupManager mgr;
    mgr.addCanister(FuelCanister(Vec3f{ 0.0f, 0.0f, 0.0f }, 10.0f));

    float rotBefore = mgr.getCanisters()[0].rotation;
    mgr.update(1.0f);
    float rotAfter = mgr.getCanisters()[0].rotation;

    REQUIRE(rotAfter > rotBefore);
}

TEST_CASE("FuelPickupManager getTotalCount reflects added canisters", "[mechanics][fuel]")
{
    FuelPickupManager mgr;
    REQUIRE(mgr.getTotalCount() == 0);
    mgr.addCanister(FuelCanister(Vec3f{ 0.0f, 0.0f, 0.0f }, 5.0f));
    mgr.addCanister(FuelCanister(Vec3f{ 1.0f, 0.0f, 0.0f }, 5.0f));
    REQUIRE(mgr.getTotalCount() == 2);
}

TEST_CASE("FuelPickupManager collects multiple canisters in one pass", "[mechanics][fuel]")
{
    FuelPickupManager mgr;
    mgr.addCanister(FuelCanister(Vec3f{ 0.0f, 0.0f, 0.0f }, 10.0f));
    mgr.addCanister(FuelCanister(Vec3f{ 0.5f, 0.0f, 0.0f }, 15.0f));

    float restored = mgr.checkCollection(Vec3f{ 0.2f, 0.0f, 0.0f }, 5.0f);
    REQUIRE(restored == Catch::Approx(25.0f));
    REQUIRE(mgr.getRemainingCount() == 0);
}
