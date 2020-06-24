#include "dun.h"
#include <faio/fafileobject.h>
#include <iostream>
#include <misc/stringops.h>
#include <serial/loader.h>
#include <stdio.h>
#include <tinyxml2.h>

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

    Dun::Dun(const tinyxml2::XMLDocument& xml)
    {
        const tinyxml2::XMLElement* mapElement = xml.FirstChildElement("map");
        int32_t width = mapElement->IntAttribute("width", -1);
        int32_t height = mapElement->IntAttribute("height", -1);
        release_assert(width > 0 && height > 0);

        const tinyxml2::XMLElement* tilsetElement = mapElement->FirstChildElement("tileset");
        release_assert(tilsetElement->IntAttribute("firstgid", -1) == 1);

        // Only allow one tileset
        release_assert(tilsetElement->NextSiblingElement("tileset") == nullptr);

        const tinyxml2::XMLElement* tilesLayer = getFirstChildWithTypeAndAttribute(mapElement, "layer", "name", "tiles");
        const tinyxml2::XMLElement* dataElement = tilesLayer->FirstChildElement("data");
        release_assert(std::string(dataElement->Attribute("encoding")) == "csv");

        std::string csvData = dataElement->GetText();

        std::vector<std::string> numbers = Misc::StringUtils::split(csvData, ',', Misc::StringUtils::SplitEmptyBehavior::StripEmpties);
        release_assert(numbers.size() == size_t(width * height));

        std::vector<int32_t> data;
        data.reserve(numbers.size());

        for (std::string& str : numbers)
        {
            Misc::StringUtils::lstrip(str);
            Misc::StringUtils::rstrip(str);
            release_assert(!str.empty());

            char* end = nullptr;
            int32_t val = int32_t(strtol(str.data(), &end, 10));
            release_assert(end == str.data() + str.size());

            data.push_back(val);
        }

        mBlocks = Misc::Array2D<int32_t>(width, height, std::move(data));
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
