#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

#include "game_mechanics.hpp"

namespace replay_export
{
    inline std::string to_csv(const std::vector<FlightFrame>& frames)
    {
        std::ostringstream out;
        out << "timestamp,position_x,position_y,position_z,velocity_x,velocity_y,velocity_z,speed,altitude\n";
        out << std::fixed << std::setprecision(4);

        for (const auto& f : frames)
        {
            out
                << f.timestamp << ','
                << f.position.x << ',' << f.position.y << ',' << f.position.z << ','
                << f.velocity.x << ',' << f.velocity.y << ',' << f.velocity.z << ','
                << f.speed << ',' << f.altitude << '\n';
        }

        return out.str();
    }

    inline bool validate_monotonic_timestamps(const std::vector<FlightFrame>& frames)
    {
        if (frames.size() < 2)
            return true;

        for (std::size_t i = 1; i < frames.size(); ++i)
        {
            if (frames[i].timestamp < frames[i - 1].timestamp)
                return false;
        }
        return true;
    }
}
