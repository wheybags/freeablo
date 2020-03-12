#include "dun.h"
#include <iostream>
#include <stdio.h>

#include <faio/fafileobject.h>
#include <serial/loader.h>

namespace Level
{
    Dun::Dun(const std::string& filename)
    {
        FAIO::FAFileObject f(filename);

        int16_t width, height;
        f.FAfread(&width, 2, 1);
        f.FAfread(&height, 2, 1);

        std::vector<int16_t> buf(width * height);
        f.FAfread(buf.data(), 2, width * height);

        std::vector<int32_t> data(buf.size());
        std::copy(buf.begin(), buf.end(), data.begin());

        mBlocks = Misc::Array2D<int32_t>(width, height, std::move(data));
    }

    Dun::Dun(Serial::Loader& loader)
    {
        uint32_t size = loader.load<uint32_t>();
        std::vector<int32_t> tmp;
        tmp.reserve(size);
        for (uint32_t i = 0; i < size; i++)
            tmp.push_back(loader.load<int32_t>());

        int32_t width = loader.load<int32_t>();
        int32_t height = loader.load<int32_t>();

        mBlocks = Misc::Array2D<int32_t>(width, height, std::move(tmp));
    }

    Dun::Dun(int32_t width, int32_t height) { resize(width, height); }

    Dun::Dun() {}

    void Dun::save(Serial::Saver& saver) const
    {
        Serial::ScopedCategorySaver cat("Dun", saver);

        uint32_t size = mBlocks.width() * mBlocks.height();
        saver.save(size);
        for (int32_t val : mBlocks)
            saver.save(val);

        saver.save(mBlocks.width());
        saver.save(mBlocks.height());
    }

    void Dun::resize(int32_t width, int32_t height) { mBlocks = Misc::Array2D<int32_t>(width, height); }

    Dun Dun::getTown(const Dun& sector1, const Dun& sector2, const Dun& sector3, const Dun& sector4)
    {
        Dun town(48, 48);

        for (int32_t x = 0; x < sector3.width(); x++)
        {
            for (int32_t y = 0; y < sector3.height(); y++)
            {
                town.get(0 + x, 23 + y) = sector3.get(x, y);
            }
        }

        for (int32_t x = 0; x < sector4.width(); x++)
        {
            for (int32_t y = 0; y < sector4.height(); y++)
            {
                town.get(0 + x, 0 + y) = sector4.get(x, y);
            }
        }

        for (int32_t x = 0; x < sector1.width(); x++)
        {
            for (int32_t y = 0; y < sector1.height(); y++)
            {
                town.get(23 + x, 23 + y) = sector1.get(x, y);
            }
        }

        for (int32_t x = 0; x < sector2.width(); x++)
        {
            for (int32_t y = 0; y < sector2.height(); y++)
            {
                town.get(23 + x, 0 + y) = sector2.get(x, y);
            }
        }

        return town;
    }
}
