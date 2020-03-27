#pragma once
#include <cel/celframe.h>

namespace Cel
{
    std::vector<CelFrame> loadTilesetImage(const std::string& celPath, const std::string& minPath, bool top);
}