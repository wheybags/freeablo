#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace Cel
{
    struct Colour
    {
        Colour(uint8_t r, uint8_t g, uint8_t b, bool visible) : r(r), g(g), b(b), a(visible ? 255 : 0) {}
        Colour() = default;

        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;
        uint8_t a = 0;
    };

    static_assert(sizeof(Colour) == 4);

    class Pal
    {
    public:
        Pal();
        explicit Pal(const std::string& filename);

        const Colour& operator[](size_t index) const;

    private:
        std::vector<Colour> contents;
    };
}
