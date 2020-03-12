#pragma once
#include <misc/array2d.h>
#include <misc/misc.h>
#include <stdint.h>
#include <string>
#include <vector>

namespace Serial
{
    class Loader;
    class Saver;
}

namespace Level
{
    class Dun
    {

    public:
        Dun(const std::string&);
        Dun(Serial::Loader& loader);
        Dun();
        Dun(int32_t width, int32_t height);

        void save(Serial::Saver& saver) const;

        static Dun getTown(const Dun& sector1, const Dun& sector2, const Dun& sector3, const Dun& sector4);

        bool pointIsValid(int32_t x, int32_t y) const { return mBlocks.pointIsValid(x, y); }
        int32_t& get(int32_t x, int32_t y) { return mBlocks.get(x, y); }
        const int32_t& get(int32_t x, int32_t y) const { return mBlocks.get(x, y); }

        int32_t width() const { return mBlocks.width(); }
        int32_t height() const { return mBlocks.height(); }

    private:
        void resize(int32_t width, int32_t height);

        Misc::Array2D<int32_t> mBlocks;
    };
}
