#include <catch2/catch_amalgamated.hpp>

#include "../main/game_mechanics.hpp"
#include "../main/replay_export.hpp"

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

TEST_CASE("Power-up manager can report and reset active effects", "[mechanics][powerup]")
{
    PowerUpManager manager;
    manager.activatePowerUp(PowerUpType::SPEED_BOOST);
    manager.activatePowerUp(PowerUpType::SHIELD);

    REQUIRE(manager.getActiveEffectCount() == 2);

    manager.resetAllEffects();
    REQUIRE(manager.getActiveEffectCount() == 0);
    REQUIRE_FALSE(manager.isPowerUpActive(PowerUpType::SPEED_BOOST));
    REQUIRE_FALSE(manager.isPowerUpActive(PowerUpType::SHIELD));
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
    REQUIRE(recorder.getRecording().front().timestamp == Catch::Approx(0.2f));
    REQUIRE(recorder.getRecording().back().timestamp == Catch::Approx(0.3f));

    recorder.clear();
    REQUIRE(recorder.getRecording().empty());

    recorder.stopRecording();
    REQUIRE_FALSE(recorder.isRecording());
}

TEST_CASE("Flight recorder ignores writes when maxFrames is zero", "[mechanics][recorder]")
{
    FlightRecorder recorder(0);
    recorder.startRecording();
    recorder.recordFrame({ 0.5f, Vec3f{ 0.0f, 0.0f, 0.0f }, Vec3f{ 0.0f, 0.0f, 0.0f }, 0.0f, 0.0f });
    REQUIRE(recorder.getRecording().empty());
}

TEST_CASE("Replay exporter emits CSV with stable schema", "[mechanics][replay]")
{
    std::vector<FlightFrame> frames;
    frames.push_back({ 0.1f, Vec3f{ 1.0f, 2.0f, 3.0f }, Vec3f{ 0.2f, 0.3f, 0.4f }, 5.0f, 2.0f });
    frames.push_back({ 0.2f, Vec3f{ 1.5f, 2.5f, 3.5f }, Vec3f{ 0.3f, 0.4f, 0.5f }, 6.0f, 2.5f });

    auto csv = replay_export::to_csv(frames);

    REQUIRE(csv.find("timestamp,position_x,position_y,position_z,velocity_x,velocity_y,velocity_z,speed,altitude") != std::string::npos);
    REQUIRE(csv.find("0.1000,1.0000,2.0000,3.0000,0.2000,0.3000,0.4000,5.0000,2.0000") != std::string::npos);
    REQUIRE(csv.find("0.2000,1.5000,2.5000,3.5000,0.3000,0.4000,0.5000,6.0000,2.5000") != std::string::npos);
}

TEST_CASE("Replay timestamp validator catches disorder", "[mechanics][replay]")
{
    std::vector<FlightFrame> ordered;
    ordered.push_back({ 0.1f, Vec3f{ 0.0f, 0.0f, 0.0f }, Vec3f{ 0.0f, 0.0f, 0.0f }, 0.0f, 0.0f });
    ordered.push_back({ 0.3f, Vec3f{ 0.0f, 0.0f, 0.0f }, Vec3f{ 0.0f, 0.0f, 0.0f }, 0.0f, 0.0f });

    std::vector<FlightFrame> disordered = ordered;
    disordered.push_back({ 0.2f, Vec3f{ 0.0f, 0.0f, 0.0f }, Vec3f{ 0.0f, 0.0f, 0.0f }, 0.0f, 0.0f });

    REQUIRE(replay_export::validate_monotonic_timestamps(ordered));
    REQUIRE_FALSE(replay_export::validate_monotonic_timestamps(disordered));
}
