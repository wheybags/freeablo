#ifndef PAL_H
#define PAL_H

#include <stdint.h>
#include <vector>
#include <string>

struct colour
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    bool visible;

    colour(uint8_t _r, uint8_t _g, uint8_t _b, bool _visible)
    {
        r = _r; g = _g; b = _b; visible = _visible;
    }

    colour(){ visible = true; }
};


class Pal
{
    public:
        Pal(std::string filename);
        
        colour& operator[](size_t index);

    private:
        std::vector<colour> contents;
};

#endif
