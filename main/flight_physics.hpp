// Flight physics calculations extracted from the main loop.

#ifndef FLIGHT_PHYSICS_HPP
#define FLIGHT_PHYSICS_HPP

#include <cmath>

namespace flight_physics {

constexpr float kPi = 3.141592653589793f;
constexpr float kFlightTimeMax = 30.0f;
constexpr float kArcRadius = 35.0f;
constexpr float kArcHeight = 25.0f;
constexpr float kAngleFactor = 0.2f * kPi / 180.0f;

struct FlightState
{
    float shootTime;
    float offsetY;
    float offsetZ;
};

struct FlightOutput
{
    float remoteX;
    float remoteY;
    float remoteZ;
    float theta; // arc angle
};

inline FlightOutput compute_trajectory(FlightState const& fs)
{
    FlightOutput out{};
    out.theta = kAngleFactor * fs.shootTime * fs.shootTime;
    out.remoteY = kArcHeight * std::sin(out.theta) + fs.offsetY;
    out.remoteX = kArcRadius - kArcRadius * std::cos(out.theta);
    out.remoteZ = fs.offsetZ;
    return out;
}

struct SteerInput
{
    bool left;
    bool right;
    bool up;
    bool down;
    float steerSpeed;
    float steerUpFactor;
    float dt;
};

struct SteerResult
{
    float deltaZ;
    float deltaY;
    float fuelCost;
    bool steered;
};

inline SteerResult compute_steer(SteerInput const& in)
{
    SteerResult r{};
    if (in.left) { r.deltaZ -= in.steerSpeed * in.dt; r.fuelCost += in.dt; r.steered = true; }
    if (in.right) { r.deltaZ += in.steerSpeed * in.dt; r.fuelCost += in.dt; r.steered = true; }
    if (in.up) { r.deltaY += in.steerSpeed * in.steerUpFactor * in.dt; r.fuelCost += in.dt; r.steered = true; }
    if (in.down) { r.deltaY -= in.steerSpeed * in.steerUpFactor * in.dt; r.fuelCost += in.dt; r.steered = true; }
    return r;
}

struct ClampLimits
{
    float minZ;
    float maxZ;
    float minY;
    float maxY;
};

inline void clamp_offsets(float& offsetY, float& offsetZ, ClampLimits const& lim)
{
    if (offsetZ < lim.minZ) offsetZ = lim.minZ;
    if (offsetZ > lim.maxZ) offsetZ = lim.maxZ;
    if (offsetY < lim.minY) offsetY = lim.minY;
    if (offsetY > lim.maxY) offsetY = lim.maxY;
}

inline float advance_shoot_time(float current, float dt, float speedScale)
{
    float next = current + dt * 6.0f * speedScale;
    if (next > kFlightTimeMax) next = kFlightTimeMax;
    return next;
}

} // namespace flight_physics

#endif // FLIGHT_PHYSICS_HPP
