#pragma once
#include <cel/celframe.h>

namespace Cel
{
    enum class TilesetImagePart
    {
        Top,
        Bottom,
        Whole
    };
    std::vector<CelFrame> loadTilesetImage(const std::string& celPath, const std::string& minPath, TilesetImagePart part);
}