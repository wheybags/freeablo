#pragma once
#include "cel/pal.h"
#include "levelobjects.h"
#include "misc.h"
#include <cel/celframe.h>
#include <cstdint>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

struct SDL_Surface;

namespace Render
{
    typedef const AtlasTextureEntry* Sprite;
}

namespace Level
{
    class Level;
}

namespace Render
{
    extern int32_t WIDTH;
    extern int32_t HEIGHT;

    enum class TileHalf
    {
        left,
        right,
    };

    // Tile measured in indexes on tile grid
    struct Tile
    {
        Misc::Point pos;
        TileHalf half;
        Tile(int32_t xArg, int32_t yArg, TileHalf halfArg = TileHalf::left) : pos(xArg, yArg), half(halfArg) {}
        Tile(Misc::Point pos, TileHalf halfArg = TileHalf::left) : pos(pos), half(halfArg) {}
    };

    /**
     * @brief Render settings for initialization.
     */
    struct RenderSettings
    {
        int32_t windowWidth;
        int32_t windowHeight;
        bool fullscreen;
    };

    void init(const std::string& title, const RenderSettings& settings);

    const std::string& getWindowTitle();
    void setWindowTitle(const std::string& title);

    void quit();

    void resize(size_t w, size_t h);
    RenderSettings getWindowSize();

    void draw();

    void clear(int r = 0, int g = 0, int b = 255);

    extern RenderInstance* mainRenderInstance;
    extern CommandQueue* mainCommandQueue;
}
