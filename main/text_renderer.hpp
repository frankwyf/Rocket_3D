#pragma once

#include <array>
#include <cstring>
#include <cctype>
#include <cstddef>
#include <vector>

namespace text_renderer
{
    struct Vertex2D
    {
        float x, y;
        float r, g, b;
    };

    inline void push_rect(std::vector<Vertex2D>& v, float x, float y, float w, float h, float r, float g, float b)
    {
        Vertex2D v0{ x, y, r, g, b };
        Vertex2D v1{ x + w, y, r, g, b };
        Vertex2D v2{ x + w, y - h, r, g, b };
        Vertex2D v3{ x, y - h, r, g, b };
        v.push_back(v0); v.push_back(v1); v.push_back(v2);
        v.push_back(v0); v.push_back(v2); v.push_back(v3);
    }

    inline std::array<unsigned char, 7> glyph_rows(char c)
    {
        auto rows = std::array<unsigned char, 7>{ 0,0,0,0,0,0,0 };
        switch (static_cast<char>(std::toupper(static_cast<unsigned char>(c))))
        {
        case 'A': rows = { 0x0E,0x11,0x11,0x1F,0x11,0x11,0x11 }; break;
        case 'B': rows = { 0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E }; break;
        case 'C': rows = { 0x0E,0x11,0x10,0x10,0x10,0x11,0x0E }; break;
        case 'D': rows = { 0x1E,0x11,0x11,0x11,0x11,0x11,0x1E }; break;
        case 'E': rows = { 0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F }; break;
        case 'F': rows = { 0x1F,0x10,0x10,0x1E,0x10,0x10,0x10 }; break;
        case 'G': rows = { 0x0E,0x11,0x10,0x13,0x11,0x11,0x0E }; break;
        case 'H': rows = { 0x11,0x11,0x11,0x1F,0x11,0x11,0x11 }; break;
        case 'I': rows = { 0x1F,0x04,0x04,0x04,0x04,0x04,0x1F }; break;
        case 'J': rows = { 0x07,0x02,0x02,0x02,0x12,0x12,0x0C }; break;
        case 'K': rows = { 0x11,0x12,0x14,0x18,0x14,0x12,0x11 }; break;
        case 'L': rows = { 0x10,0x10,0x10,0x10,0x10,0x10,0x1F }; break;
        case 'M': rows = { 0x11,0x1B,0x15,0x15,0x11,0x11,0x11 }; break;
        case 'N': rows = { 0x11,0x19,0x15,0x13,0x11,0x11,0x11 }; break;
        case 'O': rows = { 0x0E,0x11,0x11,0x11,0x11,0x11,0x0E }; break;
        case 'P': rows = { 0x1E,0x11,0x11,0x1E,0x10,0x10,0x10 }; break;
        case 'Q': rows = { 0x0E,0x11,0x11,0x11,0x15,0x12,0x0D }; break;
        case 'R': rows = { 0x1E,0x11,0x11,0x1E,0x14,0x12,0x11 }; break;
        case 'S': rows = { 0x0F,0x10,0x10,0x0E,0x01,0x01,0x1E }; break;
        case 'T': rows = { 0x1F,0x04,0x04,0x04,0x04,0x04,0x04 }; break;
        case 'U': rows = { 0x11,0x11,0x11,0x11,0x11,0x11,0x0E }; break;
        case 'V': rows = { 0x11,0x11,0x11,0x11,0x11,0x0A,0x04 }; break;
        case 'W': rows = { 0x11,0x11,0x11,0x15,0x15,0x15,0x0A }; break;
        case 'X': rows = { 0x11,0x11,0x0A,0x04,0x0A,0x11,0x11 }; break;
        case 'Y': rows = { 0x11,0x11,0x0A,0x04,0x04,0x04,0x04 }; break;
        case 'Z': rows = { 0x1F,0x01,0x02,0x04,0x08,0x10,0x1F }; break;
        case '0': rows = { 0x0E,0x11,0x13,0x15,0x19,0x11,0x0E }; break;
        case '1': rows = { 0x04,0x0C,0x04,0x04,0x04,0x04,0x0E }; break;
        case '2': rows = { 0x0E,0x11,0x01,0x02,0x04,0x08,0x1F }; break;
        case '3': rows = { 0x1E,0x01,0x01,0x0E,0x01,0x01,0x1E }; break;
        case '4': rows = { 0x02,0x06,0x0A,0x12,0x1F,0x02,0x02 }; break;
        case '5': rows = { 0x1F,0x10,0x10,0x1E,0x01,0x01,0x1E }; break;
        case '6': rows = { 0x0E,0x10,0x10,0x1E,0x11,0x11,0x0E }; break;
        case '7': rows = { 0x1F,0x01,0x02,0x04,0x08,0x08,0x08 }; break;
        case '8': rows = { 0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E }; break;
        case '9': rows = { 0x0E,0x11,0x11,0x0F,0x01,0x01,0x0E }; break;
        case ' ': rows = { 0,0,0,0,0,0,0 }; break;
        case '-': rows = { 0,0,0,0x1F,0,0,0 }; break;
        case ':': rows = { 0,0x04,0x04,0,0x04,0x04,0 }; break;
        case '.': rows = { 0,0,0,0,0,0x06,0x06 }; break;
        case ',': rows = { 0,0,0,0,0x06,0x06,0x02 }; break;
        case '+': rows = { 0,0x04,0x04,0x1F,0x04,0x04,0 }; break;
        case '/': rows = { 0x01,0x02,0x04,0x08,0x10,0,0 }; break;
        default: rows = { 0x1F,0x11,0x02,0x04,0x08,0x11,0x1F }; break;
        }
        return rows;
    }

    inline float measure_text_width_px(char const* text, float sizePx)
    {
        if (!text)
            return 0.f;
        float pixel = sizePx / 7.f;
        float advance = pixel * 6.f;
        return std::strlen(text) * advance;
    }

    inline std::vector<Vertex2D> build_text_vertices(
        int windowWidth,
        int windowHeight,
        float xPx,
        float yPx,
        float sizePx,
        float r, float g, float b,
        char const* text
    )
    {
        std::vector<Vertex2D> verts;
        if (!text || windowWidth <= 0 || windowHeight <= 0)
            return verts;
        float pixel = sizePx / 7.f;
        float advance = pixel * 6.f;
        verts.reserve(std::strlen(text) * 5 * 7 * 6);

        auto to_ndc_x = [&](float px) { return (px / float(windowWidth)) * 2.f - 1.f; };
        auto to_ndc_y = [&](float py) { return 1.f - (py / float(windowHeight)) * 2.f; };

        for (std::size_t i = 0; text[i] != '\0'; ++i)
        {
            auto rows = glyph_rows(text[i]);
            float gx = xPx + static_cast<float>(i) * advance;
            for (int row = 0; row < 7; ++row)
            {
                for (int col = 0; col < 5; ++col)
                {
                    if ((rows[row] & (1u << (4 - col))) == 0)
                        continue;
                    float x0 = to_ndc_x(gx + col * pixel);
                    float y0 = to_ndc_y(yPx + row * pixel);
                    float x1 = to_ndc_x(gx + (col + 1) * pixel);
                    float y1 = to_ndc_y(yPx + (row + 1) * pixel);
                    push_rect(verts, x0, y0, x1 - x0, y0 - y1, r, g, b);
                }
            }
        }

        return verts;
    }
}
