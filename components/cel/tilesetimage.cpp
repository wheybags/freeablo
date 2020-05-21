#include <cel/celfile.h>
#include <cel/tilesetimage.h>
#include <level/min.h>

namespace Cel
{
    static void drawMinPillar(CelFrame& frame, int x, int y, const std::vector<int16_t>& pillar, std::vector<Image>& tilesetCel, TilesetImagePart part);
    static void drawMinTile(CelFrame& frame, std::vector<Image>& tilesetCel, int x, int y, int16_t leftImageIndex, int16_t rightImageIndex);

    std::vector<CelFrame> loadTilesetImage(const std::string& celPath, const std::string& minPath, TilesetImagePart part)
    {
        CelFile tilesetCelDecoder(celPath);
        std::vector<Image> tilesetCel = tilesetCelDecoder.decode();

        Level::Min min(minPath);

        std::vector<CelFrame> retval;

        for (size_t i = 0; i < min.size() - 1; i++)
        {
            CelFrame frame(64, 256);
            drawMinPillar(frame, 0, 0, min[i], tilesetCel, part);
            retval.emplace_back(std::move(frame));
        }

        return retval;
    }

    static void drawMinPillar(CelFrame& frame, int x, int y, const std::vector<int16_t>& pillar, std::vector<Image>& tilesetCel, TilesetImagePart part)
    {
        // compensate for maps using 5-row min files
        if (pillar.size() == 10)
            y += 3 * 32;

        int32_t i = 0;
        int32_t limit = 0;

        switch (part)
        {
            case TilesetImagePart::Top:
                i = 0;
                limit = int32_t(pillar.size()) - 2;
                break;
            case TilesetImagePart::Bottom:
                i = int32_t(pillar.size()) - 2;
                limit = int32_t(pillar.size());
                y += i * 16;
                break;
            case TilesetImagePart::Whole:
                i = 0;
                limit = int32_t(pillar.size());
        }

        // Each iteration draw one row of the min
        for (; i < limit; i += 2)
        {
            int16_t leftImageIndex = (pillar[i] & 0x0FFF) - 1;
            int16_t rightImageIndex = (pillar[i + 1] & 0x0FFF) - 1;

            drawMinTile(frame, tilesetCel, x, y, leftImageIndex, rightImageIndex);

            y += 32; // down 32 each row
        }
    }

    static void drawMinTile(CelFrame& frame, std::vector<Image>& tilesetCel, int x, int y, int16_t leftImageIndex, int16_t rightImageIndex)
    {
        if (leftImageIndex != -1)
            tilesetCel[leftImageIndex].blitTo(frame, x, y);

        if (rightImageIndex != -1)
            tilesetCel[rightImageIndex].blitTo(frame, x + 32, y);
    }
}
