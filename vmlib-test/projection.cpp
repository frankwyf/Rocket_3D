#include <catch2/catch_amalgamated.hpp>

#include "../vmlib/mat44.hpp"

// See mat44-rotation.cpp first.

TEST_CASE("Perspective projection", "[mat44]")
{
	static constexpr float kEps_ = 1e-6f;
	static constexpr float kEps_large = 1e-3f;

	using namespace Catch::Matchers;

	SECTION("Standard")
	{
		auto const proj = make_perspective_projection(
			60.f * 3.1415926f / 180.f,
			1280 / float(720),
			0.1f, 100.f
		);

		REQUIRE_THAT(proj(0, 0), WithinAbs(0.974279, kEps_));
		REQUIRE_THAT(proj(0, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(proj(0, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(proj(0, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(proj(1, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(proj(1, 1), WithinAbs(1.732051f, kEps_));
		REQUIRE_THAT(proj(1, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(proj(1, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(proj(2, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(proj(2, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(proj(2, 2), WithinAbs(-1.002002f, kEps_));
		REQUIRE_THAT(proj(2, 3), WithinAbs(-0.200200f, kEps_));
		REQUIRE_THAT(proj(3, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(proj(3, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(proj(3, 2), WithinAbs(-1.f, kEps_));
		REQUIRE_THAT(proj(3, 3), WithinAbs(0.f, kEps_));
	}

	SECTION("Different values")
	{
		auto const proj = make_perspective_projection(
			45.f * 3.1415926f / 180.f,  // Different field of view
			1920 / float(1080),         // Different aspect ratio
			0.5f, 50.f                  // Different near and far planes
		);

		REQUIRE_THAT(proj(0, 0), WithinAbs(1.358, 1e-2f));
		REQUIRE_THAT(proj(0, 1), WithinAbs(0.0, kEps_));
		REQUIRE_THAT(proj(0, 2), WithinAbs(0.0, kEps_));
		REQUIRE_THAT(proj(0, 3), WithinAbs(0.0, kEps_));

		REQUIRE_THAT(proj(1, 0), WithinAbs(0.0, kEps_));
		REQUIRE_THAT(proj(1, 1), WithinAbs(2.41421, 1e-2f));
		REQUIRE_THAT(proj(1, 2), WithinAbs(0.0, kEps_));
		REQUIRE_THAT(proj(1, 3), WithinAbs(0.0, kEps_));

		REQUIRE_THAT(proj(2, 0), WithinAbs(0.0, kEps_));
		REQUIRE_THAT(proj(2, 1), WithinAbs(0.0, kEps_));
		REQUIRE_THAT(proj(2, 2), WithinAbs(-1.0203, 1e-3f));
		REQUIRE_THAT(proj(2, 3), WithinAbs(-1.0101, 1e-3f));

		REQUIRE_THAT(proj(3, 0), WithinAbs(0.0, kEps_));
		REQUIRE_THAT(proj(3, 1), WithinAbs(0.0, kEps_));
		REQUIRE_THAT(proj(3, 2), WithinAbs(-1.0, kEps_));
		REQUIRE_THAT(proj(3, 3), WithinAbs(0.0, kEps_));
	}

	SECTION("Very Small Field of View")
	{
		auto const proj1 = make_perspective_projection(
			1.f * 3.1415926f / 180.f,    // Very small field of view
			1.0f / 1000.0f,               // Very large aspect ratio
			0.0001f, 10000.f              // Very small near and very large far planes
		);

		REQUIRE_THAT(proj1(0, 0), WithinAbs(114588.64844f, kEps_));
		REQUIRE_THAT(proj1(1, 1), WithinAbs(114.58865f, kEps_large));
		REQUIRE_THAT(proj1(2, 2), WithinAbs(-1.0000001f, kEps_));
		REQUIRE_THAT(proj1(2, 3), WithinAbs(-0.0002f, kEps_));
		REQUIRE_THAT(proj1(3, 2), WithinAbs(-1.f, kEps_));
		REQUIRE_THAT(proj1(3, 3), WithinAbs(0.f, kEps_large));
	}

	SECTION("Very Large Field of View")
	{
		auto const proj2 = make_perspective_projection(
			179.f * 3.1415926f / 180.f,  // Very large field of view
			1000.0f / 1.0f,               // Large aspect ratio
			1.0f, 1000.f                  // Moderate near and far planes
		);

		// Computed Exact Values:
		REQUIRE_THAT(proj2(0, 0), WithinAbs(0.000001, kEps_large));
		REQUIRE_THAT(proj2(1, 1), WithinAbs(0.00873, kEps_large));
		REQUIRE_THAT(proj2(2, 2), WithinAbs(-1.002, kEps_large));
		REQUIRE_THAT(proj2(2, 3), WithinAbs(-2.002, kEps_large));
		REQUIRE_THAT(proj2(3, 2), WithinAbs(-1.f, kEps_));
		REQUIRE_THAT(proj2(3, 3), WithinAbs(0.f, kEps_));
	}

	SECTION("Extreme Aspect Ratio")
	{
		auto const proj3 = make_perspective_projection(
			60.f * 3.1415926f / 180.f,  // Moderate field of view
			1.0f / 100000.0f,            // Very small aspect ratio
			1.0f, 100.f                  // Moderate near and far planes
		);

		// Computed Exact Values:
		REQUIRE_THAT(proj3(0, 0), WithinAbs(173205.10938f, kEps_));
		REQUIRE_THAT(proj3(1, 1), WithinAbs(1.73205f, kEps_large));
		REQUIRE_THAT(proj3(2, 2), WithinAbs(-1.0202f, kEps_large));
		REQUIRE_THAT(proj3(2, 3), WithinAbs(-2.0202f, kEps_large));
		REQUIRE_THAT(proj3(3, 2), WithinAbs(-1.f, kEps_));
		REQUIRE_THAT(proj3(3, 3), WithinAbs(0.f, kEps_));
	}

}