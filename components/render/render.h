#pragma once

#include <stdint.h>

#include <string>
#include <utility>
#include <vector>

#include <functional>

#include "misc.h"
#include "misc/point.h"

#include "cel/pal.h"
#include <boost/optional.hpp>

struct SDL_Cursor;
struct SDL_Surface;
class SpriteGroup;

namespace Render
{
    typedef void* Sprite;
    typedef SDL_Cursor* FACursor;
    typedef SDL_Surface* FASurface;
} // namespace Render

namespace Cel
{
    struct Colour;
}

#include "levelobjects.h"

#include "nuklear_sdl_gl3.h"

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

    // Tile mesasured in indexes on tile grid
    struct Tile
    {
        Misc::Point pos;
        TileHalf half;
        Tile(int32_t xArg, int32_t yArg, TileHalf halfArg = TileHalf::left) : pos(xArg, yArg), half(halfArg) {}
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

    struct NuklearGraphicsContext
    {
        nk_gl_device dev;
        nk_font_atlas atlas;
    };

    void init(const std::string& title, const RenderSettings& settings, NuklearGraphicsContext& nuklearGraphics, nk_context* nk_ctx);

    void setWindowSize(const RenderSettings& settings);

    void destroyNuklearGraphicsContext(NuklearGraphicsContext& nuklearGraphics);
    void quit();

    void resize(size_t w, size_t h);
    RenderSettings getWindowSize();
    void drawGui(NuklearFrameDump& dump, SpriteCacheBase* cache);

    bool getImageInfo(const std::string& path, std::vector<int32_t>& widths, std::vector<int32_t>& heights, int32_t& animLength);
    void drawCursor(Sprite s);
    SpriteGroup* loadSprite(const std::string& path, bool hasTrans, size_t transR, size_t transG, size_t transB);
    SpriteGroup* loadVanimSprite(const std::string& path, size_t vAnim, bool hasTrans, size_t transR, size_t transG, size_t transB);
    SpriteGroup* loadResizedSprite(
        const std::string& path, size_t width, size_t height, size_t tileWidth, size_t tileHeight, bool hasTrans, size_t transR, size_t transG, size_t transB);
    SpriteGroup* loadCelToSingleTexture(const std::string& path);
    SpriteGroup* loadSprite(const uint8_t* source, size_t width, size_t height);
    SpriteGroup* loadTiledTexture(const std::string& sourcePath, size_t width, size_t height, bool hasTrans, size_t transR, size_t transG, size_t transB);
    SpriteGroup* loadNonCelSprite(const std::string& path);
    SDL_Surface* loadNonCelImageTrans(const std::string& path, const std::string& extension, bool hasTrans, size_t transR, size_t transG, size_t transB);
    Cel::Colour getPixel(const SDL_Surface* s, int x, int y);

    void draw();

    void handleEvents();

    void drawSprite(const Sprite& sprite, int32_t x, int32_t y, boost::optional<Cel::Colour> highlightColor = boost::none);

    struct RocketFATex
    {
        size_t animLength;
        size_t spriteIndex;
        size_t index;
        bool needsImmortal;
    };

    void spriteSize(const Sprite& sprite, int32_t& w, int32_t& h);

    SpriteGroup* loadTilesetSprite(const std::string& celPath, const std::string& minPath, bool top);
    void drawLevel(const Level::Level& level,
                   size_t minTopsHandle,
                   size_t minBottomsHandle,
                   SpriteCacheBase* cache,
                   LevelObjects& objs,
                   LevelObjects& items,
                   int32_t x1,
                   int32_t y1,
                   int32_t x2,
                   int32_t y2,
                   size_t dist);

    Tile getTileByScreenPos(size_t x, size_t y, int32_t x1, int32_t y1, int32_t x2, int32_t y2, size_t dist);

    void clear(int r = 0, int g = 0, int b = 255);
} // namespace Render
