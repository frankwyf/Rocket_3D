// Warning overlay helpers: determine visual warning state based on gameplay metrics.

#ifndef WARNING_OVERLAY_HPP
#define WARNING_OVERLAY_HPP

#include <cmath>

namespace warning_overlay {

enum class WarningLevel
{
    NONE,
    CAUTION,  // medium urgency
    CRITICAL  // high urgency
};

struct WarningState
{
    WarningLevel fuelWarning;
    WarningLevel timerWarning;
    float flashIntensity; // 0..1 pulse for visual effect
};

inline WarningLevel classify_fuel(float fuelFraction)
{
    if (fuelFraction <= 0.10f) return WarningLevel::CRITICAL;
    if (fuelFraction <= 0.25f) return WarningLevel::CAUTION;
    return WarningLevel::NONE;
}

inline WarningLevel classify_timer(float remainingSeconds)
{
    if (remainingSeconds <= 5.0f) return WarningLevel::CRITICAL;
    if (remainingSeconds <= 15.0f) return WarningLevel::CAUTION;
    return WarningLevel::NONE;
}

inline float compute_flash(float gameTime, WarningLevel level)
{
    if (level == WarningLevel::NONE) return 0.0f;
    float freq = (level == WarningLevel::CRITICAL) ? 6.0f : 3.0f;
    return 0.5f + 0.5f * std::sin(gameTime * freq);
}

inline WarningState evaluate(float fuelFraction, float remainingTimer, float gameTime)
{
    WarningState ws{};
    ws.fuelWarning = classify_fuel(fuelFraction);
    ws.timerWarning = classify_timer(remainingTimer);

    WarningLevel worst = ws.fuelWarning > ws.timerWarning ? ws.fuelWarning : ws.timerWarning;
    ws.flashIntensity = compute_flash(gameTime, worst);
    return ws;
}

} // namespace warning_overlay

#endif // WARNING_OVERLAY_HPP
