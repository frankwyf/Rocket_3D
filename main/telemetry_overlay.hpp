#pragma once

#include <array>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <vector>

namespace telemetry_overlay
{
    struct TelemetrySample
    {
        float timeS = 0.0f;
        float altitude = 0.0f;
        float speed = 0.0f;
        float fuel = 100.0f;
    };

    struct OverlayPoint
    {
        float x = 0.0f;
        float y = 0.0f;
    };

    struct OverlayColor
    {
        float r = 1.0f;
        float g = 1.0f;
        float b = 1.0f;
    };

    enum class Metric
    {
        Altitude,
        Speed,
        Fuel,
        Risk
    };

    enum class Trend
    {
        Down,
        Stable,
        Up
    };

    class TelemetryHistory
    {
    public:
        explicit TelemetryHistory(std::size_t capacity = 180)
            : capacity_(std::max<std::size_t>(1, capacity))
        {
            samples_.reserve(capacity_);
        }

        void push(const TelemetrySample& sample)
        {
            if (samples_.size() == capacity_)
            {
                samples_.erase(samples_.begin());
            }
            samples_.push_back(sample);
        }

        void clear()
        {
            samples_.clear();
        }

        std::size_t size() const { return samples_.size(); }
        std::size_t capacity() const { return capacity_; }
        bool empty() const { return samples_.empty(); }

        const std::vector<TelemetrySample>& data() const { return samples_; }

    private:
        std::size_t capacity_;
        std::vector<TelemetrySample> samples_;
    };

    inline float clamp01(float v)
    {
        return std::max(0.0f, std::min(1.0f, v));
    }

    inline float compute_risk_score(const TelemetrySample& s)
    {
        float speedRisk = clamp01((s.speed - 5.0f) / 18.0f);
        float altitudeRisk = clamp01((2.0f - s.altitude) / 2.0f);
        float fuelRisk = clamp01((25.0f - s.fuel) / 25.0f);

        return clamp01(0.55f * speedRisk + 0.25f * altitudeRisk + 0.20f * fuelRisk);
    }

    inline OverlayColor risk_to_color(float risk)
    {
        float t = clamp01(risk);
        OverlayColor c;
        c.r = 0.20f + 0.80f * t;
        c.g = 0.95f - 0.80f * t;
        c.b = 0.30f - 0.22f * t;
        return c;
    }

    inline float select_metric_value(const TelemetrySample& s, Metric metric)
    {
        switch (metric)
        {
        case Metric::Altitude: return s.altitude;
        case Metric::Speed: return s.speed;
        case Metric::Fuel: return s.fuel;
        case Metric::Risk: return compute_risk_score(s);
        default: return 0.0f;
        }
    }

    inline std::vector<OverlayPoint> build_metric_polyline(
        const TelemetryHistory& history,
        Metric metric,
        float left,
        float top,
        float width,
        float height)
    {
        std::vector<OverlayPoint> points;
        const auto& samples = history.data();
        if (samples.size() < 2 || width <= 0.0f || height <= 0.0f)
            return points;

        points.reserve(samples.size());

        float minValue = select_metric_value(samples.front(), metric);
        float maxValue = minValue;

        for (const auto& s : samples)
        {
            float v = select_metric_value(s, metric);
            minValue = std::min(minValue, v);
            maxValue = std::max(maxValue, v);
        }

        float span = std::max(0.0001f, maxValue - minValue);

        for (std::size_t i = 0; i < samples.size(); ++i)
        {
            float x = left + width * (static_cast<float>(i) / static_cast<float>(samples.size() - 1));
            float normalized = (select_metric_value(samples[i], metric) - minValue) / span;
            float y = top - normalized * height;
            points.push_back({ x, y });
        }

        return points;
    }

    inline float compute_metric_average(const TelemetryHistory& history, Metric metric, std::size_t tailCount = 0)
    {
        const auto& samples = history.data();
        if (samples.empty())
            return 0.0f;

        std::size_t count = tailCount == 0 ? samples.size() : std::min(tailCount, samples.size());
        std::size_t start = samples.size() - count;

        float sum = 0.0f;
        for (std::size_t i = start; i < samples.size(); ++i)
            sum += select_metric_value(samples[i], metric);

        return sum / static_cast<float>(count);
    }

    inline Trend detect_metric_trend(const TelemetryHistory& history, Metric metric, std::size_t tailCount = 6, float epsilon = 0.001f)
    {
        const auto& samples = history.data();
        if (samples.size() < 2)
            return Trend::Stable;

        std::size_t count = std::max<std::size_t>(2, std::min(tailCount, samples.size()));
        std::size_t start = samples.size() - count;

        float first = select_metric_value(samples[start], metric);
        float last = select_metric_value(samples.back(), metric);
        float delta = last - first;

        if (delta > epsilon)
            return Trend::Up;
        if (delta < -epsilon)
            return Trend::Down;
        return Trend::Stable;
    }

    inline std::array<OverlayPoint, 4> make_radar_marker(float centerX, float centerY, float radius)
    {
        return {
            OverlayPoint{ centerX, centerY + radius },
            OverlayPoint{ centerX + radius, centerY },
            OverlayPoint{ centerX, centerY - radius },
            OverlayPoint{ centerX - radius, centerY }
        };
    }
}
