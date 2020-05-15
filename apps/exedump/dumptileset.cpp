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

        // save til tiles + Tiled editor tileset file
        {
            filesystem::path levelOutputDir = tilesetDir / "til_tiles" / std::to_string(tilesetLevel);

            if (levelOutputDir.exists())
                filesystem::remove_all(levelOutputDir);
            filesystem::create_directories(levelOutputDir);

            FILE* tsxFile = fopen((levelOutputDir / ("diablo_l" + std::to_string(tilesetLevel) + "_tileset.tsx")).str().c_str(), "wb");

            std::string tsxHeader =
                "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<tileset version=\"1.2\" tiledversion=\"1.3.4\" name=\"diablo_l{}\" tilewidth=\"128\" tileheight=\"288\" tilecount=\"{}\" columns=\"0\">\n"
                " <grid orientation=\"isometric\" width=\"128\" height=\"64\"/>\n";
            tsxHeader = fmt::format(tsxHeader, tilesetLevel, tilImages.size());
            fputs(tsxHeader.c_str(), tsxFile);

            for (size_t frame = 0; frame < tilImages.size(); frame++)
            {
                std::string filename;
                {
                    std::ostringstream ss;
                    ss << std::setfill('0') << std::setw(4) << (frame + 1);
                    filename = (ss.str() + ".png");
                }

                std::string tsxLine = " <tile id=\"{}\">\n"
                                      "  <image width=\"128\" height=\"288\" source=\"./{}\"/>\n"
                                      " </tile>\n";
                tsxLine = fmt::format(tsxLine, frame, filename);
                fputs(tsxLine.c_str(), tsxFile);

                Image::saveToPng(tilImages[frame], (levelOutputDir / filename).str());
            }

            fputs("</tileset>\n", tsxFile);

            fclose(tsxFile);
        }

        // save min tiles
        {
            std::vector<Image> images = Cel::loadTilesetImage(celPath, minPath, Cel::TilesetImagePart::Whole);
            filesystem::path levelOutputDir = tilesetDir / "min_tiles" / std::to_string(tilesetLevel);

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