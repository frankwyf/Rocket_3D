// Game Mechanics and Utilities for SkyRocket 3D
// Provides gameplay systems, scoring, particle effects, and additional features

#ifndef GAME_MECHANICS_HPP
#define GAME_MECHANICS_HPP

#include <vector>
#include <array>
#include <cmath>
#include <cstdint>

#include "../vmlib/vec3.hpp"
#include "../vmlib/vec4.hpp"
#include "../vmlib/mat44.hpp"

// ============================================================================
// Particle System
// ============================================================================

struct Particle
{
    Vec3f position;
    Vec3f velocity;
    Vec3f color;
    float lifetime;
    float maxLifetime;
    float size;
};

class ParticleEmitter
{
public:
    ParticleEmitter(Vec3f origin, std::size_t maxParticles = 1000)
        : origin_(origin), particles_(), maxParticles_(maxParticles)
    {
        particles_.reserve(maxParticles);
    }

    void emit(const Particle& baseParticle, std::size_t count)
    {
        for (std::size_t i = 0; i < count && particles_.size() < maxParticles_; ++i)
        {
            particles_.push_back(baseParticle);
        }
    }

    void update(float deltaTime)
    {
        for (auto& particle : particles_)
        {
            particle.lifetime -= deltaTime;
            particle.position += particle.velocity * deltaTime;
            particle.velocity.y -= 9.8f * deltaTime; // Gravity
        }

        // Remove dead particles
        particles_.erase(
            std::remove_if(particles_.begin(), particles_.end(),
                [](const Particle& p) { return p.lifetime <= 0.0f; }),
            particles_.end()
        );
    }

    const std::vector<Particle>& getParticles() const { return particles_; }
    std::vector<Particle>& getParticles() { return particles_; }

private:
    Vec3f origin_;
    std::vector<Particle> particles_;
    std::size_t maxParticles_;
};

// ============================================================================
// Score and Statistics System
// ============================================================================

struct FlightStatistics
{
    float distanceTraveled = 0.0f;
    float maxAltitude = 0.0f;
    float maxSpeed = 0.0f;
    float flightTime = 0.0f;
    int landingAccuracy = 0; // 0-100
    float fuelUsed = 0.0f;
    int landingAttempts = 0;
    int successfulLandings = 0;
};

class ScoreManager
{
public:
    ScoreManager() : score_(0), multiplier_(1.0f), stats_() {}

    void updateScore(float basePoints)
    {
        score_ += static_cast<int>(basePoints * multiplier_);
    }

    void updateMultiplier(float newMultiplier)
    {
        multiplier_ = newMultiplier;
    }

    void recordLanding(int accuracy, bool successful)
    {
        stats_.landingAttempts++;
        if (successful)
        {
            stats_.successfulLandings++;
            updateScore(100.0f * (accuracy / 100.0f));
        }
        else
        {
            updateScore(10.0f);
        }
    }

    int getScore() const { return score_; }
    float getMultiplier() const { return multiplier_; }
    const FlightStatistics& getStatistics() const { return stats_; }
    FlightStatistics& getStatistics() { return stats_; }

private:
    int score_;
    float multiplier_;
    FlightStatistics stats_;
};

// ============================================================================
// Power-up System
// ============================================================================

enum class PowerUpType
{
    SPEED_BOOST,      // 2x speed for 5 seconds
    SHIELD,           // Protect from collision damage
    SLOW_TIME,        // 0.5x time scale
    FUEL_RECOVERY,    // Restore fuel
    AUTO_PILOT,       // Automatic stabilization
};

struct PowerUp
{
    PowerUpType type;
    Vec3f position;
    float duration;
    bool active;
    float rotation;

    PowerUp(PowerUpType t, Vec3f pos, float dur)
        : type(t), position(pos), duration(dur), active(true), rotation(0.0f) {}
};

class PowerUpManager
{
public:
    PowerUpManager() : powerUps_(), activeEffects_() {}

    void addPowerUp(const PowerUp& powerUp)
    {
        powerUps_.push_back(powerUp);
    }

    void activatePowerUp(PowerUpType type)
    {
        activeEffects_[static_cast<int>(type)] = 0.0f;
    }

    void update(float deltaTime)
    {
        for (auto& powerUp : powerUps_)
        {
            powerUp.rotation += deltaTime * 2.0f;
        }

        for (int i = 0; i < 5; ++i)
        {
            if (activeEffects_[i] >= 0.0f)
            {
                activeEffects_[i] += deltaTime;
            }
        }
    }

    bool isPowerUpActive(PowerUpType type) const
    {
        int idx = static_cast<int>(type);
        return activeEffects_[idx] >= 0.0f;
    }

    const std::vector<PowerUp>& getPowerUps() const { return powerUps_; }
    std::vector<PowerUp>& getPowerUps() { return powerUps_; }

private:
    std::vector<PowerUp> powerUps_;
    std::array<float, 5> activeEffects_ = {-1.0f, -1.0f, -1.0f, -1.0f, -1.0f};
};

// ============================================================================
// Challenge System
// ============================================================================

enum class ChallengeType
{
    PRECISION_LANDING,    // Land within target zone
    TIME_TRIAL,          // Complete objective within time limit
    OBSTACLE_COURSE,     // Navigate through waypoints
    FUEL_LIMIT,          // Complete with fuel limit
    SPEED_RUN,           // Reach destination as fast as possible
};

struct Challenge
{
    ChallengeType type;
    std::string name;
    std::string description;
    Vec3f targetPosition;
    float targetRadius;
    float timeLimit;
    int reward;
    bool completed;

    Challenge(ChallengeType t, const std::string& n, const std::string& d,
              Vec3f target, float radius, float time, int rew)
        : type(t), name(n), description(d), targetPosition(target),
          targetRadius(radius), timeLimit(time), reward(rew), completed(false) {}
};

class ChallengeManager
{
public:
    ChallengeManager() : challenges_(), currentChallenge_(0), elapsed_(0.0f) {}

    void addChallenge(const Challenge& challenge)
    {
        challenges_.push_back(challenge);
    }

    void update(float deltaTime, Vec3f currentPosition)
    {
        if (currentChallenge_ < challenges_.size())
        {
            elapsed_ += deltaTime;
            auto& challenge = challenges_[currentChallenge_];

            if (!challenge.completed)
            {
                // Check if within target radius
                Vec3f diff = currentPosition - challenge.targetPosition;
                float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

                if (distance <= challenge.targetRadius)
                {
                    challenge.completed = true;
                }

                // Check time limit
                if (elapsed_ > challenge.timeLimit && challenge.type == ChallengeType::TIME_TRIAL)
                {
                    advanceChallenge();
                }
            }
        }
    }

    void advanceChallenge()
    {
        if (currentChallenge_ < challenges_.size())
        {
            currentChallenge_++;
            elapsed_ = 0.0f;
        }
    }

    const Challenge* getCurrentChallenge() const
    {
        if (currentChallenge_ < challenges_.size())
            return &challenges_[currentChallenge_];
        return nullptr;
    }

    int getCompletedCount() const
    {
        int count = 0;
        for (const auto& c : challenges_)
            if (c.completed) count++;
        return count;
    }

private:
    std::vector<Challenge> challenges_;
    std::size_t currentChallenge_;
    float elapsed_;
};

// ============================================================================
// Waypoint Navigation System
// ============================================================================

struct Waypoint
{
    Vec3f position;
    float radius;
    int index;
    bool visited;

    Waypoint(Vec3f pos, float r, int idx)
        : position(pos), radius(r), index(idx), visited(false) {}
};

class WaypointNavigator
{
public:
    WaypointNavigator() : waypoints_(), nextWaypoint_(0) {}

    void addWaypoint(const Waypoint& waypoint)
    {
        waypoints_.push_back(waypoint);
    }

    void checkPosition(Vec3f currentPosition)
    {
        if (nextWaypoint_ >= waypoints_.size())
            return;

        auto& waypoint = waypoints_[nextWaypoint_];
        Vec3f diff = currentPosition - waypoint.position;
        float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

        if (distance <= waypoint.radius)
        {
            waypoint.visited = true;
            nextWaypoint_++;
        }
    }

    const Waypoint* getNextWaypoint() const
    {
        if (nextWaypoint_ < waypoints_.size())
            return &waypoints_[nextWaypoint_];
        return nullptr;
    }

    int getProgress() const
    {
        return static_cast<int>(nextWaypoint_);
    }

    int getTotalWaypoints() const
    {
        return static_cast<int>(waypoints_.size());
    }

    bool isComplete() const
    {
        return nextWaypoint_ >= waypoints_.size();
    }

private:
    std::vector<Waypoint> waypoints_;
    std::size_t nextWaypoint_;
};

// ============================================================================
// Flight Data Recorder
// ============================================================================

struct FlightFrame
{
    float timestamp;
    Vec3f position;
    Vec3f velocity;
    float speed;
    float altitude;
};

class FlightRecorder
{
public:
    FlightRecorder(std::size_t maxFrames = 10000)
        : frames_(), maxFrames_(maxFrames), isRecording_(false) {}

    void startRecording()
    {
        frames_.clear();
        isRecording_ = true;
    }

    void stopRecording()
    {
        isRecording_ = false;
    }

    void recordFrame(const FlightFrame& frame)
    {
        if (isRecording_ && frames_.size() < maxFrames_)
            frames_.push_back(frame);
    }

    const std::vector<FlightFrame>& getRecording() const { return frames_; }
    bool isRecording() const { return isRecording_; }

private:
    std::vector<FlightFrame> frames_;
    std::size_t maxFrames_;
    bool isRecording_;
};

#endif // GAME_MECHANICS_HPP
