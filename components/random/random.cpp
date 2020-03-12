#include "random.h"
#include <locale>
#include <misc/assert.h>
#include <serial/loader.h>
#include <sstream>

namespace Random
{
    DummyRng DummyRng::instance;

    Rng::~Rng() = default;
    static_assert(mt19937::max() == std::numeric_limits<uint32_t>::max(), "");

    void RngMersenneTwister::load(Serial::Loader& loader)
    {
        std::string rngStr = loader.load<std::string>();

        std::stringstream ss(rngStr);
        ss.imbue(std::locale::classic());
        ss >> mRng;
    }

    void RngMersenneTwister::save(Serial::Saver& saver) const
    {
        std::stringstream ss;
        ss.imbue(std::locale::classic());
        ss << mRng;

        std::string rngStr = ss.str();
        saver.save(rngStr);
    }

    // This should generate random numbers weighted towards min, ie, if you were to generate a bunch of samples
    // between eg 0 and 5, and plot them in a histogram, it should look roughly like:
    // 0: ************
    // 1: **********
    // 2: *******
    // 3: *****
    // 4: ***
    // 5: **
    int32_t RngMersenneTwister::squaredRand(int32_t _min, int32_t _max)
    {
        debug_assert(_min >= 0);
        debug_assert(_max >= _min);

        // Approximates the following floating point code:
        //
        // double val = gen() / double(gen.max());
        // return min + int32_t(val * val * (max - min));
        //
        // This wouldn't work with integers, as it needs values in 0-1 range, so we multiply by a big number at the start,
        // and divide by a big number at the end, and also do all the work in 64 bit ints.
        // The double code is commented out inline below, unscaledFinaldD should be the same as unscaledFinal (or at least almost the same).

        uint64_t scale = 10000;

        uint64_t generatorMax = mRng.max();
        uint64_t min = uint64_t(_min);
        uint64_t max = uint64_t(_max);

        uint64_t sample = mRng();

        // double dsample = sample;
        uint64_t scaledSamp = sample * scale;

        // double sampleIn01Range = dsample / double(generatorMax);
        uint64_t scaledSampleIn01Range = scaledSamp / generatorMax;

        // double final = sampleIn01Range * sampleIn01Range * double(max-min);
        uint64_t scaledFinal = scaledSampleIn01Range * scaledSampleIn01Range * (max - min);

        // double unscaledFinalD = min + final;
        uint64_t unscaledFinal =
            min + scaledFinal / (scale * scale); // using scale squared here because we squared scaledSampleIn01Range above, so the scale is present twice

        return int32_t(unscaledFinal);
    }

    int32_t RngMersenneTwister::randomInRange(int32_t _min, int32_t _max)
    {
        if (_max == _min)
            return _min;

        // do everything in 64-bit to prevent overflow, for eg randomInRange(INT_MIN, INT_MAX)
        int64_t min = _min;
        int64_t max = _max;

        int64_t val = uint32_t(mRng());
        int64_t range = (max + 1) - min;
        int64_t modVal = val % range;
        int64_t final = min + modVal;
        return int32_t(final);
    }
}
