// Centralized scoring calculation helpers.

#ifndef SCORING_HELPERS_HPP
#define SCORING_HELPERS_HPP

namespace scoring {

struct MissionBonus
{
    int baseScore;
    int timeBonus;
    int fuelBonus;
    int comboBonus;
    int total;
};

inline MissionBonus compute_mission_score(float remainingTimer, float remainingFuel, int comboTotalBonus)
{
    MissionBonus b{};
    b.baseScore = 500;
    b.timeBonus = static_cast<int>(remainingTimer * 10.f);
    b.fuelBonus = static_cast<int>(remainingFuel * 5.f);
    b.comboBonus = comboTotalBonus;
    b.total = b.baseScore + b.timeBonus + b.fuelBonus + b.comboBonus;
    return b;
}

inline int compute_target_score(int comboBonus)
{
    return 100 + comboBonus;
}

inline int compute_boss_gate_score()
{
    return 150;
}

struct LaunchRating
{
    char const* label;
    float fraction; // 0.0 to 1.0
};

inline LaunchRating rate_launch(int score, int maxPossible)
{
    if (maxPossible <= 0)
        return { "---", 0.0f };
    float frac = static_cast<float>(score) / static_cast<float>(maxPossible);
    if (frac >= 0.9f) return { "S", frac };
    if (frac >= 0.75f) return { "A", frac };
    if (frac >= 0.5f) return { "B", frac };
    if (frac >= 0.25f) return { "C", frac };
    return { "D", frac };
}

} // namespace scoring

#endif // SCORING_HELPERS_HPP
