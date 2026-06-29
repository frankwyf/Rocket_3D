#include <catch2/catch_amalgamated.hpp>

#include "../main/game_mechanics.hpp"

TEST_CASE("Power-up activation expires after configured duration", "[mechanics][powerup]")
{
    PowerUpManager manager;
    manager.activatePowerUp(PowerUpType::SPEED_BOOST);

    REQUIRE(manager.isPowerUpActive(PowerUpType::SPEED_BOOST));

    manager.update(2.0f);
    REQUIRE(manager.isPowerUpActive(PowerUpType::SPEED_BOOST));
    REQUIRE(manager.getRemainingTime(PowerUpType::SPEED_BOOST) == Catch::Approx(3.0f));

    manager.update(3.1f);
    REQUIRE_FALSE(manager.isPowerUpActive(PowerUpType::SPEED_BOOST));
    REQUIRE(manager.getRemainingTime(PowerUpType::SPEED_BOOST) == Catch::Approx(0.0f));
}

TEST_CASE("Challenge manager marks challenge completed at target", "[mechanics][challenge]")
{
    ChallengeManager manager;
    manager.addChallenge(Challenge(
        ChallengeType::PRECISION_LANDING,
        "Landing",
        "Land in target",
        Vec3f{ 10.0f, 2.0f, -1.0f },
        1.5f,
        30.0f,
        300
    ));

    manager.update(0.16f, Vec3f{ 10.4f, 2.1f, -1.3f });

    REQUIRE(manager.getCompletedCount() == 1);
}

TEST_CASE("Flight recorder obeys record state and max capacity", "[mechanics][recorder]")
{
    FlightRecorder recorder(2);

    recorder.recordFrame({ 0.0f, Vec3f{ 0.0f, 0.0f, 0.0f }, Vec3f{ 0.0f, 0.0f, 0.0f }, 0.0f, 0.0f });
    REQUIRE(recorder.getRecording().empty());

    recorder.startRecording();
    recorder.recordFrame({ 0.1f, Vec3f{ 1.0f, 2.0f, 3.0f }, Vec3f{ 1.0f, 0.0f, 0.0f }, 1.0f, 2.0f });
    recorder.recordFrame({ 0.2f, Vec3f{ 2.0f, 3.0f, 4.0f }, Vec3f{ 1.0f, 0.0f, 0.0f }, 1.0f, 3.0f });
    recorder.recordFrame({ 0.3f, Vec3f{ 3.0f, 4.0f, 5.0f }, Vec3f{ 1.0f, 0.0f, 0.0f }, 1.0f, 4.0f });

    REQUIRE(recorder.getRecording().size() == 2);

    recorder.stopRecording();
    REQUIRE_FALSE(recorder.isRecording());
}
