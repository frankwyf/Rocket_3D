// Audio System Interfaces for SkyRocket 3D
// Provides audio management infrastructure for future sound implementation

#ifndef AUDIO_SYSTEM_HPP
#define AUDIO_SYSTEM_HPP

#include <string>
#include <vector>
#include <memory>

// ============================================================================
// Audio Event Types
// ============================================================================

enum class AudioEventType
{
    ROCKET_THRUST,       // Rocket engine sound
    LANDING_SUCCESS,     // Successful landing chime
    LANDING_FAILURE,     // Failed landing buzz
    COLLISION,           // Collision impact sound
    POWERUP_PICKUP,      // Power-up collected sound
    MENU_SELECT,         // UI selection sound
    AMBIENT_WIND,        // Background wind
    BOOST_ACTIVATED,     // Speed boost sound
};

// ============================================================================
// Audio Manager Interface
// ============================================================================

class AudioManager
{
public:
    virtual ~AudioManager() = default;

    // Initialize audio system
    virtual bool initialize() = 0;

    // Shutdown audio system
    virtual void shutdown() = 0;

    // Play sound effect at position
    virtual void playSFX(AudioEventType type, float volume = 1.0f) = 0;

    // Play 3D positional audio
    virtual void play3D(AudioEventType type, float x, float y, float z, float volume = 1.0f) = 0;

    // Set listener position (camera position)
    virtual void setListenerPosition(float x, float y, float z) = 0;

    // Set master volume (0.0 - 1.0)
    virtual void setMasterVolume(float volume) = 0;

    // Get current master volume
    virtual float getMasterVolume() const = 0;

    // Pause/Resume all audio
    virtual void pause() = 0;
    virtual void resume() = 0;

    // Update audio system (call every frame)
    virtual void update() = 0;
};

// ============================================================================
// Null Audio Manager (No-op implementation)
// ============================================================================

class NullAudioManager : public AudioManager
{
public:
    bool initialize() override { return true; }
    void shutdown() override {}
    void playSFX(AudioEventType, float) override {}
    void play3D(AudioEventType, float, float, float, float) override {}
    void setListenerPosition(float, float, float) override {}
    void setMasterVolume(float) override {}
    float getMasterVolume() const override { return 1.0f; }
    void pause() override {}
    void resume() override {}
    void update() override {}
};

#endif // AUDIO_SYSTEM_HPP
