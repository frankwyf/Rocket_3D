#pragma once

#include "simple_mesh.hpp"
#include "rocket.hpp"
#include "../vmlib/mat44.hpp"
#include "../vmlib/vec3.hpp"

namespace ship_builder
{
    inline constexpr float kPi = 3.1415926f;

    inline SimpleMeshData build_rocket_body()
    {
        auto cyl = make_cylinder(true, 8, { 1.f, 0.4f, 0.f },
            make_rotation_z(kPi / 2.f) * make_scaling(0.3f, 0.3f, 0.3f));
        auto cy2 = make_cylinder(true, 6, { 0.0f, 0.3f, 0.6f },
            make_rotation_z(kPi / 2.f) * make_scaling(0.6f, 0.2f, 0.2f) * make_translation({ 0.5f, 0.f, 0.f }));
        auto cy3 = make_cylinder(true, 10, { 0.0f, 0.6f, 0.3f },
            make_rotation_z(kPi / 2.f) * make_scaling(0.5f, 0.15f, 0.15f) * make_translation({ 1.8f, 0.f, 0.f }));
        auto cy4 = make_cylinder(true, 8, { 1.f, 0.f, 0.6f },
            make_rotation_z(kPi / 2.f) * make_scaling(0.6f, 0.1f, 0.1f) * make_translation({ 2.3f, 0.f, 0.f }));

        struct FinSpec { float angleMultiplier; Vec3f color; };
        FinSpec fins[] = {
            { 1.f, { 0.3f, 0.8f, 0.6f } },
            { 3.f, { 0.3f, 0.4f, 0.2f } },
            { 5.f, { 0.3f, 0.8f, 0.6f } },
            { 7.f, { 0.3f, 0.2f, 0.6f } },
            { 9.f, { 0.3f, 0.8f, 0.6f } },
            { 11.f, { 0.3f, 0.4f, 0.2f } },
        };

        auto ship = concatenate(std::move(cyl), cy2);
        ship = concatenate(std::move(ship), cy3);
        ship = concatenate(std::move(ship), cy4);

        for (auto const& fin : fins)
        {
            auto f = make_cylinder(true, 4, fin.color,
                make_rotation_y(kPi / 6.f * fin.angleMultiplier) *
                make_rotation_z(kPi / 2.f) *
                make_scaling(0.1f, 0.1f, 0.1f) *
                make_translation({ 5.f, 2.8f, 0.f }));
            ship = concatenate(std::move(ship), f);
        }

        auto nozzle = make_cone(true, 8, { 1.f, 0.f, 0.6f },
            make_rotation_z(kPi / 2.f) * make_scaling(0.3f, 0.1f, 0.1f) * make_translation({ -1.f, 0.f, 0.f }));
        ship = concatenate(std::move(ship), nozzle);

        return ship;
    }

    inline SimpleMeshData build_exhaust_trail(int segments = 6)
    {
        SimpleMeshData trail;
        for (int i = 0; i < segments; ++i)
        {
            auto cone = make_cone(true, 8, { 1.f, 0.f, 0.6f },
                make_rotation_z(kPi / 2.f) *
                make_scaling(0.3f, 0.1f, 0.1f) *
                make_translation({ -1.f - static_cast<float>(i), 0.f, 0.f }));
            if (i == 0)
                trail = std::move(cone);
            else
                trail = concatenate(std::move(trail), cone);
        }
        return trail;
    }

    inline SimpleMeshData build_mission_beacon()
    {
        auto coinOuter = make_cylinder(true, 28, { 1.0f, 0.84f, 0.16f }, make_scaling(0.44f, 0.08f, 0.44f));
        auto coinInner = make_cylinder(true, 24, { 1.0f, 0.98f, 0.45f }, make_scaling(0.22f, 0.09f, 0.22f));
        auto coinStripeA = make_cylinder(true, 12, { 1.0f, 0.98f, 0.78f },
            make_scaling(0.07f, 0.10f, 0.30f) * make_rotation_y(kPi / 4.f));
        auto coinStripeB = make_cylinder(true, 12, { 1.0f, 0.98f, 0.78f },
            make_scaling(0.07f, 0.10f, 0.30f) * make_rotation_y(-kPi / 4.f));
        auto beacon = concatenate(std::move(coinOuter), coinInner);
        beacon = concatenate(std::move(beacon), coinStripeA);
        beacon = concatenate(std::move(beacon), coinStripeB);
        return beacon;
    }

    inline SimpleMeshData build_obstacle_mesh()
    {
        auto top = make_cone(true, 14, { 0.95f, 0.25f, 0.22f }, make_scaling(0.45f, 0.72f, 0.45f));
        auto base = make_cylinder(true, 12, { 0.72f, 0.07f, 0.07f },
            make_scaling(0.43f, 0.10f, 0.43f) * make_translation({ 0.f, -0.7f, 0.f }));
        return concatenate(std::move(top), base);
    }

    inline SimpleMeshData build_boss_gate_mesh()
    {
        auto outer = make_cylinder(true, 26, { 0.25f, 0.95f, 0.96f }, make_scaling(0.95f, 0.08f, 0.95f));
        auto inner = make_cylinder(true, 22, { 0.10f, 0.55f, 0.60f }, make_scaling(0.63f, 0.09f, 0.63f));
        return concatenate(std::move(outer), inner);
    }

    inline SimpleMeshData build_fuel_canister()
    {
        auto body = make_cylinder(true, 16, { 0.15f, 0.85f, 0.30f }, make_scaling(0.25f, 0.40f, 0.25f));
        auto cap = make_cylinder(true, 12, { 0.10f, 0.60f, 0.20f },
            make_scaling(0.30f, 0.06f, 0.30f) * make_translation({ 0.f, 0.4f, 0.f }));
        auto ring = make_cylinder(true, 14, { 0.90f, 0.95f, 0.30f },
            make_scaling(0.28f, 0.04f, 0.28f));
        auto mesh = concatenate(std::move(body), cap);
        return concatenate(std::move(mesh), ring);
    }
}
