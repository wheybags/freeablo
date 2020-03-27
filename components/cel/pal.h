#pragma once
#include <Image/image.h>
#include <cstdint>
#include <string>
#include <vector>

namespace Cel
{
    using Colour = ByteColour;

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
