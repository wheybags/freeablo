
#pragma once

#include <stdint.h>
#include <string>
#include <vector>

#include <misc/helper2d.h>
#include <misc/misc.h>

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

        void save(Serial::Saver& saver);

        static Dun getTown(const Dun& sector1, const Dun& sector2, const Dun& sector3, const Dun& sector4);

        Misc::Helper2D<Dun, int32_t&> operator[](int32_t x);
        Misc::Helper2D<const Dun, const int32_t&> operator[](int32_t x) const;

        int32_t width() const;
        int32_t height() const;

    private:
        void resize(int32_t width, int32_t height);

        std::vector<int32_t> mBlocks;
        int32_t mWidth;
        int32_t mHeight;

        friend const int32_t& get(int32_t x, int32_t y, const Dun& dun);
        friend int32_t& get(int32_t x, int32_t y, Dun& dun);
    };
}
