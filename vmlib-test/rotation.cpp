#include <catch2/catch_amalgamated.hpp>

#include "../vmlib/mat44.hpp"

TEST_CASE("4x4 rotation around Z axis", "[rotation][mat44]")
{
	static constexpr float kEps_ = 1e-6f;

	using namespace Catch::Matchers;

	// Simple check: rotating zero degrees should yield an idenity matrix
	SECTION("Identity")
	{
		auto const identity = make_rotation_z(0.f);

		REQUIRE_THAT(identity(0, 0), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(identity(0, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(0, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(0, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(identity(1, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(1, 1), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(identity(1, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(1, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(identity(2, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(2, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(2, 2), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(identity(2, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(identity(3, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(3, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(3, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(3, 3), WithinAbs(1.f, kEps_));
	}

	// Rotating 90 degrees = pi/2 radians.
	SECTION("90 degrees")
	{
		auto const right = make_rotation_z(3.1415926f / 2.f);

		REQUIRE_THAT(right(0, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(0, 1), WithinAbs(-1.f, kEps_));
		REQUIRE_THAT(right(0, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(0, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(right(1, 0), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(right(1, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(1, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(1, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(right(2, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(2, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(2, 2), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(right(2, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(right(3, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(3, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(3, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(3, 3), WithinAbs(1.f, kEps_));
	}

	// Rotating -90 degrees = -pi/2 radians.
	SECTION("-90 degrees")
	{
		auto const right = make_rotation_z(-3.1415926f / 2.f);

		REQUIRE_THAT(right(0, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(0, 1), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(right(0, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(0, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(right(1, 0), WithinAbs(-1.f, kEps_));
		REQUIRE_THAT(right(1, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(1, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(1, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(right(2, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(2, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(2, 2), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(right(2, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(right(3, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(3, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(3, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(3, 3), WithinAbs(1.f, kEps_));
	}

	SECTION("180 degrees")
	{
		auto const rotation = make_rotation_z(3.1415926f);

		REQUIRE_THAT(rotation(0, 0), WithinAbs(-1.f, kEps_));
		REQUIRE_THAT(rotation(0, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(0, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(0, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(rotation(1, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(1, 1), WithinAbs(-1.f, kEps_));
		REQUIRE_THAT(rotation(1, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(1, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(rotation(2, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(2, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(2, 2), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(rotation(2, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(rotation(3, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(3, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(3, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(3, 3), WithinAbs(1.f, kEps_));
	}

	SECTION("45 degrees")
	{
		auto const rotation = make_rotation_z(3.1415926f / 4.f);

		REQUIRE_THAT(rotation(0, 0), WithinAbs(0.707107f, kEps_));
		REQUIRE_THAT(rotation(0, 1), WithinAbs(-0.707107f, kEps_));
		REQUIRE_THAT(rotation(0, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(0, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(rotation(1, 0), WithinAbs(0.707107f, kEps_));
		REQUIRE_THAT(rotation(1, 1), WithinAbs(0.707107f, kEps_));
		REQUIRE_THAT(rotation(1, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(1, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(rotation(2, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(2, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(2, 2), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(rotation(2, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(rotation(3, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(3, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(3, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(3, 3), WithinAbs(1.f, kEps_));
	}

	SECTION("30 degrees")
	{
		auto const rotation = make_rotation_z(3.1415926f / 6.f);

		REQUIRE_THAT(rotation(0, 0), WithinAbs(0.866025f, kEps_));
		REQUIRE_THAT(rotation(0, 1), WithinAbs(-0.5f, kEps_));
		REQUIRE_THAT(rotation(0, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(0, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(rotation(1, 0), WithinAbs(0.5f, kEps_));
		REQUIRE_THAT(rotation(1, 1), WithinAbs(0.866025f, kEps_));
		REQUIRE_THAT(rotation(1, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(1, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(rotation(2, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(2, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(2, 2), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(rotation(2, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(rotation(3, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(3, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(3, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(3, 3), WithinAbs(1.f, kEps_));
	}

	// -30 degrees
	SECTION("-30 degrees")
	{
		auto const rotation = make_rotation_z(-3.1415926f / 6.f);

		REQUIRE_THAT(rotation(0, 0), WithinAbs(0.866025f, kEps_));
		REQUIRE_THAT(rotation(0, 1), WithinAbs(0.5f, kEps_));
		REQUIRE_THAT(rotation(0, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(0, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(rotation(1, 0), WithinAbs(-0.5f, kEps_));
		REQUIRE_THAT(rotation(1, 1), WithinAbs(0.866025f, kEps_));
		REQUIRE_THAT(rotation(1, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(1, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(rotation(2, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(2, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(2, 2), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(rotation(2, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(rotation(3, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(3, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(3, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(3, 3), WithinAbs(1.f, kEps_));
	}

	SECTION("Non-zero angle")
	{
		float angle = 1.5f; // 1.5 radians

		auto const rotation = make_rotation_z(angle);

		// Compute the expected matrix manually
		float cosAngle = std::cos(angle);
		float sinAngle = std::sin(angle);
		float expected00 = cosAngle;
		float expected01 = -sinAngle;
		// Compute the other elements of the expected matrix

		REQUIRE_THAT(rotation(0, 0), WithinAbs(expected00, kEps_));
		REQUIRE_THAT(rotation(0, 1), WithinAbs(expected01, kEps_));
		// Verify the other elements of the matrix
	}

	SECTION("Non-zero angle, negative")
	{
		float angle = -1.5f; // -1.5 radians

		auto const rotation = make_rotation_z(angle);

		// Compute the expected matrix manually
		float cosAngle = std::cos(angle);
		float sinAngle = std::sin(angle);
		float expected00 = cosAngle;
		float expected01 = -sinAngle;
		// Compute the other elements of the expected matrix

		REQUIRE_THAT(rotation(0, 0), WithinAbs(expected00, kEps_));
		REQUIRE_THAT(rotation(0, 1), WithinAbs(expected01, kEps_));
		// Verify the other elements of the matrix
		REQUIRE_THAT(rotation(1, 0), WithinAbs(sinAngle, kEps_));
		REQUIRE_THAT(rotation(1, 1), WithinAbs(cosAngle, kEps_));
	}
}

TEST_CASE("4x4 rotation around X axis", "[rotation][mat44]") {

	static constexpr float kEps_ = 1e-6f;

	using namespace Catch::Matchers;

	// -30 degrees
	SECTION("-30 degrees X-axis")
	{
		auto const rotation = make_rotation_x(-3.1415926f / 6.f);

		REQUIRE_THAT(rotation(0, 0), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(rotation(0, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(0, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(0, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(rotation(1, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(1, 1), WithinAbs(0.866025f, kEps_));
		REQUIRE_THAT(rotation(1, 2), WithinAbs(0.5f, kEps_));
		REQUIRE_THAT(rotation(1, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(rotation(2, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(2, 1), WithinAbs(-0.5f, kEps_));
		REQUIRE_THAT(rotation(2, 2), WithinAbs(0.866025f, kEps_));
		REQUIRE_THAT(rotation(2, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(rotation(3, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(3, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(3, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(3, 3), WithinAbs(1.f, kEps_));
	}

	// Non-zero angle
	SECTION("Non-zero angle X-axis")
	{
		float angle = 1.5f; // 1.5 radians

		auto const rotation = make_rotation_x(angle);

		// Compute the expected matrix manually
		float cosAngle = std::cos(angle);
		float sinAngle = std::sin(angle);
		// Compute the other elements of the expected matrix

		REQUIRE_THAT(rotation(0, 0), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(rotation(0, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(0, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(0, 3), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(1, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(1, 1), WithinAbs(cosAngle, kEps_));
		REQUIRE_THAT(rotation(1, 2), WithinAbs(-sinAngle, kEps_));
		REQUIRE_THAT(rotation(1, 3), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(2, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(2, 1), WithinAbs(sinAngle, kEps_));
		REQUIRE_THAT(rotation(2, 2), WithinAbs(cosAngle, kEps_));
		REQUIRE_THAT(rotation(2, 3), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(3, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(3, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(3, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(3, 3), WithinAbs(1.f, kEps_));
	}

	// Non-zero angle, negative
	SECTION("Non-zero angle, negative X-axis")
	{
		float angle = -1.5f; // -1.5 radians

		auto const rotation = make_rotation_x(angle);

		// Compute the expected matrix manually
		float cosAngle = std::cos(angle);
		float sinAngle = std::sin(angle);
		// Compute the other elements of the expected matrix

		REQUIRE_THAT(rotation(0, 0), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(rotation(2, 1), WithinAbs(sinAngle, kEps_));
		REQUIRE_THAT(rotation(2, 2), WithinAbs(cosAngle, kEps_));
		REQUIRE_THAT(rotation(1, 1), WithinAbs(cosAngle, kEps_));
		REQUIRE_THAT(rotation(1, 2), WithinAbs(-sinAngle, kEps_));
		REQUIRE_THAT(rotation(2, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(2, 3), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(3, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(3, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(3, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(3, 3), WithinAbs(1.f, kEps_));
	}

	// Zero angle
	SECTION("Zero angle X-axis")
	{
		float angle = 0.0f; // 0 radians

		auto const rotation = make_rotation_x(angle);

		REQUIRE_THAT(rotation(0, 0), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(rotation(0, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(0, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(1, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(1, 1), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(rotation(1, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(2, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(2, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(2, 2), WithinAbs(1.f, kEps_));
	}

	// 45 degrees
	SECTION("45 degrees X-axis")
	{
		float angle = 0.785398f; // 45 degrees in radians

		auto const rotation = make_rotation_x(angle);

		REQUIRE_THAT(rotation(0, 0), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(rotation(0, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(0, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(1, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(1, 1), WithinAbs(0.707107f, kEps_));
		REQUIRE_THAT(rotation(1, 2), WithinAbs(-0.707107f, kEps_));
		REQUIRE_THAT(rotation(2, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(2, 1), WithinAbs(0.707107f, kEps_));
		REQUIRE_THAT(rotation(2, 2), WithinAbs(0.707107f, kEps_));
	}

	// 90 degrees
	SECTION("90 degrees X-axis")
	{
		float angle = 1.5708f; // 90 degrees in radians

		auto const rotation = make_rotation_x(angle);

		REQUIRE_THAT(rotation(0, 0), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(rotation(0, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(0, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(1, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(1, 1), WithinAbs(-0.f, 1e-2f));
		REQUIRE_THAT(rotation(1, 2), WithinAbs(-1.f, kEps_));
		REQUIRE_THAT(rotation(2, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(2, 1), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(rotation(2, 2), WithinAbs(0.f, 1e-2f));
	}

	// Multiple rotations
	SECTION("Multiple rotations X-axis")
	{
		float angle1 = 1.0472f; // 60 degrees in radians
		float angle2 = -0.523599f; // -30 degrees in radians

		auto const rotation1 = make_rotation_x(angle1);
		auto const rotation2 = make_rotation_x(angle2);

		REQUIRE_THAT(rotation1(0, 0), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(rotation1(0, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation1(0, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation1(1, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation1(1, 1), WithinAbs(0.5f, 1e-3f));
		REQUIRE_THAT(rotation1(1, 2), WithinAbs(-0.86603f, 1e-3f));
		REQUIRE_THAT(rotation1(2, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation1(2, 1), WithinAbs(0.86603f, 1e-2f));
		REQUIRE_THAT(rotation1(2, 2), WithinAbs(0.5f, 1e-2f));

		REQUIRE_THAT(rotation2(0, 0), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(rotation2(0, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation2(0, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation2(1, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation2(1, 1), WithinAbs(0.866025f, kEps_));
		REQUIRE_THAT(rotation2(1, 2), WithinAbs(0.5f, kEps_));
		REQUIRE_THAT(rotation2(2, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation2(2, 1), WithinAbs(-0.5f, kEps_));
		REQUIRE_THAT(rotation2(2, 2), WithinAbs(0.866025f, kEps_));

		auto const combinedRotation = rotation2 * rotation1;

		REQUIRE_THAT(combinedRotation(0, 0), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(combinedRotation(0, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(combinedRotation(0, 2), WithinAbs(0.f, 1e-3f));
		REQUIRE_THAT(combinedRotation(1, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(combinedRotation(1, 1), WithinAbs(0.866025f, kEps_));
		REQUIRE_THAT(combinedRotation(1, 2), WithinAbs(-0.5f, 1e-2f));
		REQUIRE_THAT(combinedRotation(2, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(combinedRotation(2, 1), WithinAbs(0.5f, 1e-2f));
		REQUIRE_THAT(combinedRotation(2, 2), WithinAbs(0.866025f, kEps_));
	}
}

TEST_CASE("4x4 rotation around Y axis", "[rotation][mat44]") {

	static constexpr float kEps_ = 1e-6f;

	using namespace Catch::Matchers;

	// -30 degrees
	SECTION("-30 degrees Y-axis")
	{
		auto const rotation = make_rotation_y(-3.1415926f / 6.f);

		REQUIRE_THAT(rotation(0, 0), WithinAbs(0.866025f, kEps_));
		REQUIRE_THAT(rotation(0, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(0, 2), WithinAbs(-0.5f, kEps_));
		REQUIRE_THAT(rotation(0, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(rotation(1, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(1, 1), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(rotation(1, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(1, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(rotation(2, 0), WithinAbs(0.5f, kEps_));
		REQUIRE_THAT(rotation(2, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(2, 2), WithinAbs(0.866025f, kEps_));
		REQUIRE_THAT(rotation(2, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(rotation(3, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(3, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(3, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(3, 3), WithinAbs(1.f, kEps_));
	}

	// Non-zero angle
	SECTION("Non-zero angle Y-axis")
	{
		float angle = 1.5f; // 1.5 radians

		auto const rotation = make_rotation_y(angle);

		// Compute the expected matrix manually
		float cosAngle = std::cos(angle);
		// Compute the other elements of the expected matrix

		REQUIRE_THAT(rotation(0, 0), WithinAbs(cosAngle, kEps_));
	}

	// Non-zero angle, negative
	SECTION("Non-zero angle, negative Y-axis")
	{
		float angle = -1.5f; // -1.5 radians

		auto const rotation = make_rotation_y(angle);

		// Compute the expected matrix manually
		float cosAngle = std::cos(angle);
		float sinAngle = std::sin(angle);
		// Compute the other elements of the expected matrix

		REQUIRE_THAT(rotation(0, 0), WithinAbs(cosAngle, kEps_));
		// Verify the other elements of the matrix
		REQUIRE_THAT(rotation(0, 2), WithinAbs(sinAngle, kEps_));
		REQUIRE_THAT(rotation(2, 0), WithinAbs(-sinAngle, kEps_));
		REQUIRE_THAT(rotation(2, 2), WithinAbs(cosAngle, kEps_));
	}

	// Zero angle
	SECTION("Zero angle Y-axis")
	{
		float angle = 0.0f; // 0 radians

		auto const rotation = make_rotation_y(angle);

		REQUIRE_THAT(rotation(0, 0), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(rotation(0, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(0, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(1, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(1, 1), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(rotation(1, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(2, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(2, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(2, 2), WithinAbs(1.f, kEps_));
	}

	// 45 degrees
	SECTION("45 degrees Y-axis")
	{
		float angle = 0.785398f; // 45 degrees in radians

		auto const rotation = make_rotation_y(angle);

		REQUIRE_THAT(rotation(0, 0), WithinAbs(0.707107f, kEps_));
		REQUIRE_THAT(rotation(0, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(0, 2), WithinAbs(0.707107f, kEps_));
		REQUIRE_THAT(rotation(1, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(1, 1), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(rotation(1, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(2, 0), WithinAbs(-0.707107f, kEps_));
		REQUIRE_THAT(rotation(2, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(2, 2), WithinAbs(0.707107f, kEps_));
	}

	// 90 degrees
	SECTION("90 degrees Y-axis")
	{
		float angle = 1.5708f; // 90 degrees in radians

		auto const rotation = make_rotation_y(angle);

		REQUIRE_THAT(rotation(0, 0), WithinAbs(0.f, 1e-3f));
		REQUIRE_THAT(rotation(0, 1), WithinAbs(0.f, 1e-3f));
		REQUIRE_THAT(rotation(0, 2), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(rotation(1, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(1, 1), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(rotation(1, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation(2, 0), WithinAbs(-1.f, 1e-3f));
		REQUIRE_THAT(rotation(2, 1), WithinAbs(-0.f, 1e-3f));
		REQUIRE_THAT(rotation(2, 2), WithinAbs(-0.f, 1e-3f));
	}

	// Multiple rotations
	SECTION("Multiple rotations Y-axis")
	{
		float angle1 = 1.0472f; // 60 degrees in radians
		float angle2 = -0.523599f; // -30 degrees in radians

		auto const rotation1 = make_rotation_y(angle1);
		auto const rotation2 = make_rotation_y(angle2);

		REQUIRE_THAT(rotation1(0, 0), WithinAbs(0.5f, 1e-3f));
		REQUIRE_THAT(rotation1(0, 1), WithinAbs(0.f, 1e-3f));
		REQUIRE_THAT(rotation1(0, 2), WithinAbs(0.866025f, 1e-3f));
		REQUIRE_THAT(rotation1(1, 0), WithinAbs(0.f, 1e-3f));
		REQUIRE_THAT(rotation1(1, 1), WithinAbs(1.f, 1e-3f));
		REQUIRE_THAT(rotation1(1, 2), WithinAbs(0.f, 1e-3f));
		REQUIRE_THAT(rotation1(2, 0), WithinAbs(-0.866025f, 1e-3f));
		REQUIRE_THAT(rotation1(2, 1), WithinAbs(0.f, 1e-3f));
		REQUIRE_THAT(rotation1(2, 2), WithinAbs(0.5f, 1e-3f));

		REQUIRE_THAT(rotation2(0, 0), WithinAbs(0.866025f, 1e-3f));
		REQUIRE_THAT(rotation2(0, 1), WithinAbs(0.f, 1e-3f));
		REQUIRE_THAT(rotation2(0, 2), WithinAbs(-0.5f, 1e-3f));
		REQUIRE_THAT(rotation2(1, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation2(1, 1), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(rotation2(1, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation2(2, 0), WithinAbs(0.5f, kEps_));
		REQUIRE_THAT(rotation2(2, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(rotation2(2, 2), WithinAbs(0.866025f, kEps_));
		REQUIRE_THAT(rotation2(3, 0), WithinAbs(0.f, 1e-3f));
		REQUIRE_THAT(rotation2(3, 1), WithinAbs(0.f, 1e-3f));
		REQUIRE_THAT(rotation2(3, 2), WithinAbs(0.f, 1e-3f));
		REQUIRE_THAT(rotation2(3, 3), WithinAbs(1.f, 1e-3f));
	}
}