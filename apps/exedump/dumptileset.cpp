#include "dumptileset.h"
#include <Image/image.h>
#include <cel/tilesetimage.h>
#include <fmt/format.h>
#include <iomanip>
#include <level/tileset.h>

void dumpTiles(filesystem::path tilesetDir)
{
    for (int32_t tilesetLevel = 0; tilesetLevel <= 4; tilesetLevel++)
    {
        std::string celPath = fmt::format("levels/l{}data/l{}.cel", tilesetLevel, tilesetLevel);
        std::string minPath = fmt::format("levels/l{}data/l{}.min", tilesetLevel, tilesetLevel);
        std::string tilPath = fmt::format("levels/l{}data/l{}.til", tilesetLevel, tilesetLevel);

        if (tilesetLevel == 0)
        {
            celPath = "levels/towndata/town.cel";
            minPath = "levels/towndata/town.min";
            tilPath = "levels/towndata/town.til";
        }

        std::vector<Image> minImages = Cel::loadTilesetImage(celPath, minPath, Cel::TilesetImagePart::Whole);

        Level::TileSet til(tilPath);

        constexpr auto tileHeight = 32;
        constexpr auto tileWidth = tileHeight * 2;
        constexpr auto staticObjectHeight = 256;

        std::vector<Image> tilImages;
        for (size_t i = 0; i < til.size(); i++)
        {
            Image image(tileWidth * 2, staticObjectHeight + tileHeight);

            Level::TilBlock block = til[i];

            auto blit = [](const Image& src, Image& dest, int32_t x, int32_t y) { src.blitTo(dest, 0, 0, src.width(), src.height(), x, y, false); };

            blit(minImages[block[0]], image, tileWidth / 2, 0);
            blit(minImages[block[2]], image, 0, tileHeight / 2);
            blit(minImages[block[1]], image, tileWidth, tileHeight / 2);
            blit(minImages[block[3]], image, tileWidth / 2, tileHeight);

            tilImages.emplace_back(std::move(image));
        }

        // save til tiles
        {
            filesystem::path levelOutputDir = tilesetDir / "min_tiles" / std::to_string(tilesetLevel);

            if (levelOutputDir.exists())
                filesystem::remove_all(levelOutputDir);
            filesystem::create_directories(levelOutputDir);

            for (size_t frame = 0; frame < tilImages.size(); frame++)
            {
                std::ostringstream ss;
                ss << std::setfill('0') << std::setw(4) << (frame + 1);
                Image::saveToPng(tilImages[frame], (levelOutputDir / (ss.str() + ".png")).str());
            }
        }

        // save min tiles
        {
            std::vector<Image> images = Cel::loadTilesetImage(celPath, minPath, Cel::TilesetImagePart::Whole);
            filesystem::path levelOutputDir = tilesetDir / "til_tiles" / std::to_string(tilesetLevel);

            if (levelOutputDir.exists())
                filesystem::remove_all(levelOutputDir);
            filesystem::create_directories(levelOutputDir);

            for (size_t frame = 0; frame < images.size(); frame++)
            {
                std::ostringstream ss;
                ss << std::setfill('0') << std::setw(4) << frame;
                Image::saveToPng(images[frame], (levelOutputDir / (ss.str() + ".png")).str());
            }
        }
    }
}