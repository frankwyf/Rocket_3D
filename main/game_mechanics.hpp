// Game Mechanics and Utilities for SkyRocket 3D
// Provides gameplay systems, scoring, particle effects, and additional features

#ifndef GAME_MECHANICS_HPP
#define GAME_MECHANICS_HPP

#include <vector>
#include <array>
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <string>

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
    PowerUpManager() : powerUps_(), activeEffects_()
    {
        activeEffects_.fill(-1.0f);
    }

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
                if (activeEffects_[i] >= getPowerUpDuration(static_cast<PowerUpType>(i)))
                    activeEffects_[i] = -1.0f;
            }
        }
    }

    bool isPowerUpActive(PowerUpType type) const
    {
        int idx = static_cast<int>(type);
        return activeEffects_[idx] >= 0.0f;
    }

    void resetAllEffects()
    {
        activeEffects_.fill(-1.0f);
    }

    int getActiveEffectCount() const
    {
        int count = 0;
        for (float timer : activeEffects_)
        {
            if (timer >= 0.0f)
                ++count;
        }
        return count;
    }

    float getRemainingTime(PowerUpType type) const
    {
        int idx = static_cast<int>(type);
        if (activeEffects_[idx] < 0.0f)
            return 0.0f;

        float duration = getPowerUpDuration(type);
        return std::max(0.0f, duration - activeEffects_[idx]);
    }

    const std::vector<PowerUp>& getPowerUps() const { return powerUps_; }
    std::vector<PowerUp>& getPowerUps() { return powerUps_; }

private:
    static float getPowerUpDuration(PowerUpType type)
    {
        switch (type)
        {
        case PowerUpType::SPEED_BOOST: return 5.0f;
        case PowerUpType::SHIELD: return 7.0f;
        case PowerUpType::SLOW_TIME: return 4.0f;
        case PowerUpType::FUEL_RECOVERY: return 2.0f;
        case PowerUpType::AUTO_PILOT: return 6.0f;
        default: return 5.0f;
        }
    }

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

    void clear()
    {
        frames_.clear();
    }

    void recordFrame(const FlightFrame& frame)
    {
        if (!isRecording_)
            return;

        if (maxFrames_ == 0)
            return;

        if (frames_.size() == maxFrames_)
            frames_.erase(frames_.begin());

        frames_.push_back(frame);
    }

    const std::vector<FlightFrame>& getRecording() const { return frames_; }
    bool isRecording() const { return isRecording_; }

private:
    std::vector<FlightFrame> frames_;
    std::size_t maxFrames_;
    bool isRecording_;
};

// ============================================================================
// Combo Tracker
// ============================================================================

class ComboTracker
{
public:
    ComboTracker(float comboWindow = 3.0f, float bonusPerLevel = 50.0f)
        : comboWindow_(comboWindow), bonusPerLevel_(bonusPerLevel),
          comboCount_(0), comboTimer_(0.0f), totalBonusAwarded_(0) {}

    int collect(float currentTime)
    {
        if (comboCount_ > 0 && (currentTime - comboTimer_) > comboWindow_)
        {
            comboCount_ = 0;
        }

        comboCount_++;
        comboTimer_ = currentTime;

        int bonus = static_cast<int>(bonusPerLevel_ * comboCount_);
        totalBonusAwarded_ += bonus;
        return bonus;
    }

    void update(float currentTime)
    {
        if (comboCount_ > 0 && (currentTime - comboTimer_) > comboWindow_)
        {
            comboCount_ = 0;
        }
    }

    void reset()
    {
        comboCount_ = 0;
        comboTimer_ = 0.0f;
        totalBonusAwarded_ = 0;
    }

    int getComboCount() const { return comboCount_; }
    int getTotalBonus() const { return totalBonusAwarded_; }
    float getComboWindow() const { return comboWindow_; }
    float getTimeSinceLast() const { return comboTimer_; }
    bool isComboActive(float currentTime) const
    {
        return comboCount_ > 0 && (currentTime - comboTimer_) <= comboWindow_;
    }

private:
    float comboWindow_;
    float bonusPerLevel_;
    int comboCount_;
    float comboTimer_;
    int totalBonusAwarded_;
};

// ============================================================================
// Fuel Pickup System
// ============================================================================

struct FuelCanister
{
    Vec3f position;
    float restoreAmount;
    bool collected;
    float rotation;

    FuelCanister(Vec3f pos, float amount)
        : position(pos), restoreAmount(amount), collected(false), rotation(0.0f) {}
};

class FuelPickupManager
{
public:
    FuelPickupManager() : canisters_() {}

    void addCanister(const FuelCanister& canister)
    {
        canisters_.push_back(canister);
    }

    float checkCollection(Vec3f rocketPos, float pickupRadius)
    {
        float totalRestored = 0.0f;
        for (auto& c : canisters_)
        {
            if (c.collected)
                continue;
            Vec3f diff = rocketPos - c.position;
            float distSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
            if (distSq < pickupRadius * pickupRadius)
            {
                c.collected = true;
                totalRestored += c.restoreAmount;
            }
        }
        return totalRestored;
    }

    void update(float deltaTime)
    {
        for (auto& c : canisters_)
        {
            if (!c.collected)
                c.rotation += deltaTime * 3.0f;
        }
    }

    void resetAll()
    {
        for (auto& c : canisters_)
            c.collected = false;
    }

    int getRemainingCount() const
    {
        int count = 0;
        for (auto const& c : canisters_)
            if (!c.collected) ++count;
        return count;
    }

    int getTotalCount() const
    {
        return static_cast<int>(canisters_.size());
    }

    const std::vector<FuelCanister>& getCanisters() const { return canisters_; }

private:
    std::vector<FuelCanister> canisters_;
};

// ============================================================================
// Wind System
// ============================================================================

class WindSystem
{
public:
    WindSystem(float baseStrength = 0.0f, float gustFrequency = 1.0f, float gustAmplitude = 0.5f)
        : baseStrength_(baseStrength), gustFrequency_(gustFrequency),
          gustAmplitude_(gustAmplitude), currentOffset_(0.0f) {}

    void setParameters(float base, float freq, float amp)
    {
        baseStrength_ = base;
        gustFrequency_ = freq;
        gustAmplitude_ = amp;
    }

    float computeOffset(float timeS) const
    {
        float gust = gustAmplitude_ * std::sin(timeS * gustFrequency_ * 2.0f)
                   + gustAmplitude_ * 0.5f * std::sin(timeS * gustFrequency_ * 4.7f);
        return baseStrength_ + gust;
    }

    void update(float timeS)
    {
        currentOffset_ = computeOffset(timeS);
    }

    float getCurrentOffset() const { return currentOffset_; }
    float getBaseStrength() const { return baseStrength_; }
    float getGustFrequency() const { return gustFrequency_; }
    float getGustAmplitude() const { return gustAmplitude_; }

private:
    float baseStrength_;
    float gustFrequency_;
    float gustAmplitude_;
    float currentOffset_;
};

// ============================================================================
// Achievement Tracker
// ============================================================================

enum class AchievementId
{
    FIRST_LAUNCH,
    FIRST_LANDING,
    COMBO_3,
    COMBO_5,
    ALL_FUEL_COLLECTED,
    NO_STEERING,
    SPEED_DEMON,
    CAMPAIGN_COMPLETE,
    COUNT
};

class AchievementTracker
{
public:
    AchievementTracker()
    {
        unlocked_.fill(false);
    }

    bool unlock(AchievementId id)
    {
        int idx = static_cast<int>(id);
        if (idx < 0 || idx >= static_cast<int>(AchievementId::COUNT))
            return false;
        if (unlocked_[idx])
            return false;
        unlocked_[idx] = true;
        return true;
    }

    bool isUnlocked(AchievementId id) const
    {
        int idx = static_cast<int>(id);
        if (idx < 0 || idx >= static_cast<int>(AchievementId::COUNT))
            return false;
        return unlocked_[idx];
    }

    int getUnlockedCount() const
    {
        int count = 0;
        for (bool u : unlocked_)
            if (u) ++count;
        return count;
    }

    int getTotalCount() const
    {
        return static_cast<int>(AchievementId::COUNT);
    }

    void reset()
    {
        unlocked_.fill(false);
    }

private:
    std::array<bool, static_cast<std::size_t>(AchievementId::COUNT)> unlocked_;
};

#endif // GAME_MECHANICS_HPP
