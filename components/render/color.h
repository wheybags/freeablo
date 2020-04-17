#pragma once

namespace Render
{
    class Color
    {
    public:
        constexpr Color() = default;
        constexpr Color(const Color&) = default;
        constexpr Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}

    public:
        float r = 1.0f;
        float g = 1.0f;
        float b = 1.0f;
        float a = 1.0f;
    };

    struct Colors
    {
        static constexpr Color white = Color(1, 1, 1, 1);
        static constexpr Color black = Color(0, 0, 0, 1);
        static constexpr Color red = Color(1, 0, 0, 1);
        static constexpr Color green = Color(0, 1, 0, 1);
        static constexpr Color blue = Color(0, 0, 1, 1);
        static constexpr Color transparent = Color(0, 0, 0, 0);
    };
}