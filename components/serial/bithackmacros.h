#include <cstdint>

template <uint32_t x> struct PopCount
{
    enum {
        a = x - ((x >> 1) & 0x55555555),
        b = (((a >> 2) & 0x33333333) + (a & 0x33333333)),
        c = (((b >> 4) + b) & 0x0f0f0f0f),
        d = c + (c >> 8),
        e = d + (d >> 16),
        result = e & 0x0000003f
    };
};

template <uint32_t x> struct Log2
{
    enum {
        a = x | (x >> 1),
        b = a | (a >> 2),
        c = b | (b >> 4),
        d = c | (c >> 8),
        e = d | (d >> 16),
        f = e >> 1,
        result = PopCount<f>::result
    };
};

template <int64_t min, int64_t max> struct BitsRequired
{
    static const uint32_t result =
        (min == max) ? 0 : (Log2<uint32_t(max - min)>::result + 1);
};

#define BITS_REQUIRED( min, max ) BitsRequired<min,max>::result // http://gafferongames.com/building-a-game-network-protocol/reading-and-writing-packets/ retrieved 22/6/16