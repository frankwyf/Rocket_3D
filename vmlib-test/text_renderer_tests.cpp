#include <catch2/catch_amalgamated.hpp>

#include "../main/text_renderer.hpp"

TEST_CASE("push_rect emits 6 vertices forming two triangles", "[text][rect]")
{
    std::vector<text_renderer::Vertex2D> verts;
    text_renderer::push_rect(verts, 0.0f, 1.0f, 0.5f, 0.3f, 1.0f, 0.5f, 0.2f);

    REQUIRE(verts.size() == 6);
    REQUIRE(verts[0].r == Catch::Approx(1.0f));
    REQUIRE(verts[0].g == Catch::Approx(0.5f));
    REQUIRE(verts[0].b == Catch::Approx(0.2f));
}

TEST_CASE("glyph_rows returns non-zero pattern for known characters", "[text][glyph]")
{
    auto rows_A = text_renderer::glyph_rows('A');
    bool anySet = false;
    for (auto r : rows_A)
    {
        if (r != 0)
            anySet = true;
    }
    REQUIRE(anySet);
}

TEST_CASE("glyph_rows is case-insensitive", "[text][glyph]")
{
    auto upper = text_renderer::glyph_rows('M');
    auto lower = text_renderer::glyph_rows('m');
    for (int i = 0; i < 7; ++i)
    {
        REQUIRE(upper[i] == lower[i]);
    }
}

TEST_CASE("Space character produces empty glyph rows", "[text][glyph]")
{
    auto rows = text_renderer::glyph_rows(' ');
    for (auto r : rows)
    {
        REQUIRE(r == 0);
    }
}

TEST_CASE("measure_text_width_px scales linearly with string length", "[text][measure]")
{
    float w1 = text_renderer::measure_text_width_px("AB", 14.0f);
    float w2 = text_renderer::measure_text_width_px("ABCD", 14.0f);
    REQUIRE(w2 == Catch::Approx(w1 * 2.0f));
}

TEST_CASE("measure_text_width_px returns zero for null", "[text][measure]")
{
    REQUIRE(text_renderer::measure_text_width_px(nullptr, 14.0f) == Catch::Approx(0.0f));
}

TEST_CASE("build_text_vertices returns non-empty buffer for printable text", "[text][build]")
{
    auto verts = text_renderer::build_text_vertices(1280, 720, 10.f, 10.f, 14.f, 1.f, 1.f, 1.f, "HI");
    REQUIRE_FALSE(verts.empty());
    REQUIRE(verts.size() % 6 == 0);
}

TEST_CASE("build_text_vertices returns empty for null or zero-size window", "[text][build]")
{
    auto v1 = text_renderer::build_text_vertices(0, 720, 10.f, 10.f, 14.f, 1.f, 1.f, 1.f, "HI");
    REQUIRE(v1.empty());

    auto v2 = text_renderer::build_text_vertices(1280, 720, 10.f, 10.f, 14.f, 1.f, 1.f, 1.f, nullptr);
    REQUIRE(v2.empty());
}

TEST_CASE("build_text_vertices vertex count scales with characters", "[text][build]")
{
    auto v1 = text_renderer::build_text_vertices(1280, 720, 0.f, 0.f, 14.f, 1.f, 1.f, 1.f, "A");
    auto v2 = text_renderer::build_text_vertices(1280, 720, 0.f, 0.f, 14.f, 1.f, 1.f, 1.f, "AB");
    REQUIRE(v2.size() > v1.size());
}
