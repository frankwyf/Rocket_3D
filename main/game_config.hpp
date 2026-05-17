// Game Configuration System
// Provides runtime configuration and settings management

#ifndef GAME_CONFIG_HPP
#define GAME_CONFIG_HPP

#include <string>
#include <unordered_map>

// ============================================================================
// Graphics Configuration
// ============================================================================

struct GraphicsConfig
{
    int screenWidth = 1280;
    int screenHeight = 720;
    bool fullscreen = false;
    bool vsync = true;
    bool antialiasing = true;
    float fieldOfView = 60.0f;
    bool shadowsEnabled = true;
    int shadowMapResolution = 2048;
};

// ============================================================================
// Gameplay Configuration
// ============================================================================

struct GameplayConfig
{
    float rocketSpeed = 5.0f;
    float rocketAcceleration = 10.0f;
    float gravity = 9.81f;
    float damping = 0.98f;
    bool invulnerableMode = false;
    bool unlimitedFuel = false;
    float difficulty = 1.0f; // 0.5 = easy, 1.0 = normal, 2.0 = hard
    bool arcadeMode = false;
};

// ============================================================================
// Audio Configuration
// ============================================================================

struct AudioConfig
{
    float masterVolume = 1.0f;
    float sfxVolume = 0.8f;
    float musicVolume = 0.6f;
    float ambientVolume = 0.5f;
    bool audioEnabled = true;
    std::string audioBackend = "null"; // "openal", "fmod", "wwise", "null"
};

// ============================================================================
// Input Configuration
// ============================================================================

struct InputConfig
{
    float mouseSensitivity = 0.01f;
    bool invertMouseY = false;
    bool gamepadEnabled = true;
    float gamepadDeadzone = 0.1f;
};

// ============================================================================
// Game Configuration Manager
// ============================================================================

class GameConfig
{
public:
    GameConfig()
        : graphics_(), gameplay_(), audio_(), input_() {}

    // Graphics settings
    const GraphicsConfig& getGraphics() const { return graphics_; }
    GraphicsConfig& getGraphics() { return graphics_; }

    // Gameplay settings
    const GameplayConfig& getGameplay() const { return gameplay_; }
    GameplayConfig& getGameplay() { return gameplay_; }

    // Audio settings
    const AudioConfig& getAudio() const { return audio_; }
    AudioConfig& getAudio() { return audio_; }

    // Input settings
    const InputConfig& getInput() const { return input_; }
    InputConfig& getInput() { return input_; }

    // Load from file (INI format)
    bool loadFromFile(const std::string& filename)
    {
        // TODO: Implement INI file loading
        return true;
    }

    // Save to file
    bool saveToFile(const std::string& filename) const
    {
        // TODO: Implement INI file saving
        return true;
    }

    // Reset to defaults
    void resetToDefaults()
    {
        graphics_ = GraphicsConfig();
        gameplay_ = GameplayConfig();
        audio_ = AudioConfig();
        input_ = InputConfig();
    }

    // Apply settings
    void applySettings()
    {
        // TODO: Apply graphics, audio, and input settings
    }

private:
    GraphicsConfig graphics_;
    GameplayConfig gameplay_;
    AudioConfig audio_;
    InputConfig input_;
};

#endif // GAME_CONFIG_HPP
