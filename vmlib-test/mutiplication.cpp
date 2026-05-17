#include <catch2/catch_amalgamated.hpp>

#include <algorithm>

#include "../vmlib/vec4.hpp"

#include "../vmlib/mat44.hpp"

TEST_CASE("Matrix multiplication", "[mat44]")
{
    Mat44f m1 = {
        1.f, 2.f, 3.f, 4.f,
        5.f, 6.f, 7.f, 8.f,
        9.f, 10.f, 11.f, 12.f,
        13.f, 14.f, 15.f, 16.f
    };

    Mat44f m2 = {
        2.f, 0.f, 0.f, 0.f,
        0.f, 2.f, 0.f, 0.f,
        0.f, 0.f, 2.f, 0.f,
        0.f, 0.f, 0.f, 2.f
    };

    Mat44f result = m1 * m2;

    REQUIRE(result(0, 0) == 2.f);
    REQUIRE(result(0, 1) == 4.f);
    REQUIRE(result(0, 2) == 6.f);
    REQUIRE(result(0, 3) == 8.f);
    REQUIRE(result(1, 0) == 10.f);
    REQUIRE(result(1, 1) == 12.f);
    REQUIRE(result(1, 2) == 14.f);
    REQUIRE(result(1, 3) == 16.f);
    REQUIRE(result(2, 0) == 18.f);
    REQUIRE(result(2, 1) == 20.f);
    REQUIRE(result(2, 2) == 22.f);
    REQUIRE(result(2, 3) == 24.f);
    REQUIRE(result(3, 0) == 26.f);
    REQUIRE(result(3, 1) == 28.f);
    REQUIRE(result(3, 2) == 30.f);
    REQUIRE(result(3, 3) == 32.f);
}

TEST_CASE("Matrix-vector multiplication", "[mat44]")
{
    Mat44f m = {
        1.f, 2.f, 3.f, 4.f,
        5.f, 6.f, 7.f, 8.f,
        9.f, 10.f, 11.f, 12.f,
        13.f, 14.f, 15.f, 16.f
    };

    Vec4f v = { 2.f, 3.f, 4.f, 5.f };

    Vec4f result = m * v;

    REQUIRE(result[0] == 40.f);
    REQUIRE(result[1] == 96.f);
    REQUIRE(result[2] == 152.f);
    REQUIRE(result[3] == 208.f);
}

TEST_CASE("Matrix transpose", "[mat44]")
{
    Mat44f m = {
        1.f, 2.f, 3.f, 4.f,
        5.f, 6.f, 7.f, 8.f,
        9.f, 10.f, 11.f, 12.f,
        13.f, 14.f, 15.f, 16.f
    };

    Mat44f transposed = transpose(m);

    REQUIRE(transposed(0, 0) == 1.f);
    REQUIRE(transposed(0, 1) == 5.f);
    REQUIRE(transposed(0, 2) == 9.f);
    REQUIRE(transposed(0, 3) == 13.f);
    REQUIRE(transposed(1, 0) == 2.f);
    REQUIRE(transposed(1, 1) == 6.f);
    REQUIRE(transposed(1, 2) == 10.f);
    REQUIRE(transposed(1, 3) == 14.f);
    REQUIRE(transposed(2, 0) == 3.f);
    REQUIRE(transposed(2, 1) == 7.f);
    REQUIRE(transposed(2, 2) == 11.f);
    REQUIRE(transposed(2, 3) == 15.f);
    REQUIRE(transposed(3, 0) == 4.f);
    REQUIRE(transposed(3, 1) == 8.f);
    REQUIRE(transposed(3, 2) == 12.f);
    REQUIRE(transposed(3, 3) == 16.f);
}

// test for special cases
TEST_CASE("Matrix transpose with extreme values", "[mat44]")
{
    Mat44f m = {
        1.0, 1.0, 1.0, 1.0,
        1.0, -1.0, 1.0, -1.0,
        1.0, 1.0, -1.0, -1.0,
        1.0, -1.0, -1.0, 1.0
    };

    Mat44f transposed = transpose(m);

    REQUIRE(transposed(0, 0) == 1.0);
    REQUIRE(transposed(0, 1) == 1.0);
    REQUIRE(transposed(0, 2) == 1.0);
    REQUIRE(transposed(0, 3) == 1.0);
    REQUIRE(transposed(1, 0) == 1.0);
    REQUIRE(transposed(1, 1) == -1.0);
    REQUIRE(transposed(1, 2) == 1.0);
    REQUIRE(transposed(1, 3) == -1.0);
    REQUIRE(transposed(2, 0) == 1.0);
    REQUIRE(transposed(2, 1) == 1.0);
    REQUIRE(transposed(2, 2) == -1.0);
    REQUIRE(transposed(2, 3) == -1.0);
    REQUIRE(transposed(3, 0) == 1.0);
    REQUIRE(transposed(3, 1) == -1.0);
    REQUIRE(transposed(3, 2) == -1.0);
    REQUIRE(transposed(3, 3) == 1.0);
}

TEST_CASE("Matrix-vector multiplication with zero vector", "[mat44]")
{
    Mat44f m = {
        1.f, 2.f, 3.f, 4.f,
        5.f, 6.f, 7.f, 8.f,
        9.f, 10.f, 11.f, 12.f,
        13.f, 14.f, 15.f, 16.f
    };

    Vec4f v = { 0.f, 0.f, 0.f, 0.f };

    Vec4f result = m * v;

    REQUIRE(result[0] == 0.f);
    REQUIRE(result[1] == 0.f);
    REQUIRE(result[2] == 0.f);
    REQUIRE(result[3] == 0.f);
}

TEST_CASE("Matrix transpose with identity matrix", "[mat44]")
{
    Mat44f m = {
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f
    };

    Mat44f transposed = transpose(m);

    REQUIRE(transposed(0, 0) == 1.f);
    REQUIRE(transposed(0, 1) == 0.f);
    REQUIRE(transposed(0, 2) == 0.f);
    REQUIRE(transposed(0, 3) == 0.f);
    REQUIRE(transposed(1, 0) == 0.f);
    REQUIRE(transposed(1, 1) == 1.f);
    REQUIRE(transposed(1, 2) == 0.f);
    REQUIRE(transposed(1, 3) == 0.f);
    REQUIRE(transposed(2, 0) == 0.f);
    REQUIRE(transposed(2, 1) == 0.f);
    REQUIRE(transposed(2, 2) == 1.f);
    REQUIRE(transposed(2, 3) == 0.f);
    REQUIRE(transposed(3, 0) == 0.f);
    REQUIRE(transposed(3, 1) == 0.f);
    REQUIRE(transposed(3, 2) == 0.f);
    REQUIRE(transposed(3, 3) == 1.f);
}

TEST_CASE("Matrix-vector multiplication with a matrix of all zeros", "[mat44]")
{
    Mat44f m = {
        0.f, 0.f, 0.f, 0.f,
        0.f, 0.f, 0.f, 0.f,
        0.f, 0.f, 0.f, 0.f,
        0.f, 0.f, 0.f, 0.f
    };

    Vec4f v = { 1.f, 2.f, 3.f, 4.f };

    Vec4f result = m * v;

    REQUIRE(result[0] == 0.f);
    REQUIRE(result[1] == 0.f);
    REQUIRE(result[2] == 0.f);
    REQUIRE(result[3] == 0.f);
}

TEST_CASE("Matrix transpose with a non-square matrix", "[mat44]")
{
    Mat44f m = {
        1.f, 2.f, 3.f, 4.f,
        5.f, 6.f, 7.f, 8.f,
        9.f, 10.f, 11.f, 12.f,
        13.f, 14.f, 15.f, 16.f
    };

    Mat44f transposed = transpose(m);

    REQUIRE(transposed(0, 0) == 1.f);
    REQUIRE(transposed(0, 1) == 5.f);
    REQUIRE(transposed(0, 2) == 9.f);
    REQUIRE(transposed(0, 3) == 13.f);
    REQUIRE(transposed(1, 0) == 2.f);
    REQUIRE(transposed(1, 1) == 6.f);
    REQUIRE(transposed(1, 2) == 10.f);
    REQUIRE(transposed(1, 3) == 14.f);
    REQUIRE(transposed(2, 0) == 3.f);
    REQUIRE(transposed(2, 1) == 7.f);
    REQUIRE(transposed(2, 2) == 11.f);
    REQUIRE(transposed(2, 3) == 15.f);
    REQUIRE(transposed(3, 0) == 4.f);
    REQUIRE(transposed(3, 1) == 8.f);
    REQUIRE(transposed(3, 2) == 12.f);
    REQUIRE(transposed(3, 3) == 16.f);
}

TEST_CASE("Matrix multiplication with zero matrix", "[mat44]")
{
    Mat44f m1 = {
        0.f, 0.f, 0.f, 0.f,
         0.f, 0.f, 0.f, 0.f,
         0.f, 0.f, 0.f, 0.f,
         0.f, 0.f, 0.f, 0.f,
    };
    Mat44f m2 = {
        1.f, 2.f, 3.f, 4.f,
        5.f, 6.f, 7.f, 8.f,
        9.f, 10.f, 11.f, 12.f,
        13.f, 14.f, 15.f, 16.f
    };

    Mat44f result = m1 * m2;

    REQUIRE(result(1, 0) == 0.f);
    REQUIRE(result(2, 0) == 0.f);
    REQUIRE(result(3, 0) == 0.f);
    REQUIRE(result(0, 1) == 0.f);
    REQUIRE(result(1, 1) == 0.f);
    REQUIRE(result(2, 1) == 0.f);
    REQUIRE(result(3, 1) == 0.f);
    REQUIRE(result(0, 2) == 0.f);
    REQUIRE(result(1, 2) == 0.f);
    REQUIRE(result(2, 2) == 0.f);
    REQUIRE(result(3, 2) == 0.f);
    REQUIRE(result(0, 3) == 0.f);
    REQUIRE(result(1, 3) == 0.f);
    REQUIRE(result(2, 3) == 0.f);
    REQUIRE(result(3, 3) == 0.f);
}

TEST_CASE("Matrix-vector multiplication with zero matrix and vector", "[mat44]")
{
    Mat44f m = {
        0.f, 0.f, 0.f, 0.f,
         0.f, 0.f, 0.f, 0.f,
         0.f, 0.f, 0.f, 0.f,
         0.f, 0.f, 0.f, 0.f,
    };
    Vec4f v = { 0.f, 0.f, 0.f, 0.f,};

    Vec4f result = m * v;

    // result should be a zero vector
    REQUIRE(result[0] == 0.f);
    REQUIRE(result[1] == 0.f);
    REQUIRE(result[2] == 0.f);
    REQUIRE(result[3] == 0.f);
}

TEST_CASE("Matrix transpose for a 1x1 matrix", "[mat44]")
{
    Mat44f m = {
        1.f, 0.f, 0.f, 0.f,
        0.f, 0.f, 0.f, 0.f,
        0.f, 0.f, 0.f, 0.f,
        0.f, 0.f, 0.f, 0.f
    };

    Mat44f transposed = transpose(m);

    // Transposing a 1x1 matrix should result in the same matrix
    REQUIRE(transposed(0,0) == 1.f);
    REQUIRE(transposed(1,0) == 0.f);
    REQUIRE(transposed(2,0) == 0.f);
    REQUIRE(transposed(3,0) == 0.f);
    REQUIRE(transposed(0,1) == 0.f);
    REQUIRE(transposed(1,1) == 0.f);
    REQUIRE(transposed(2,1) == 0.f);
    REQUIRE(transposed(3,1) == 0.f);
    REQUIRE(transposed(0,2) == 0.f);
    REQUIRE(transposed(1,2) == 0.f);
    REQUIRE(transposed(2,2) == 0.f);
    REQUIRE(transposed(3,2) == 0.f);
    REQUIRE(transposed(0,3) == 0.f);
    REQUIRE(transposed(1,3) == 0.f);
    REQUIRE(transposed(2,3) == 0.f);
    REQUIRE(transposed(3,3) == 0.f);
}