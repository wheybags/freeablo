#ifndef PAL_H
#define PAL_H

#include <stdint.h>
#include <vector>
#include <string>

struct Colour
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    bool visible;

    Colour(uint8_t _r, uint8_t _g, uint8_t _b, bool _visible)
    {
        r = _r; g = _g; b = _b; visible = _visible;
    }

    Colour(){ visible = true; }
};


class Pal
{
    public:
        Pal(std::string filename);
        
        const Colour& operator[](size_t index) const;

    private:
        std::vector<Colour> contents;
};

#endif
