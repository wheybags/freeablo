#ifndef RENDER_H
#define RENDER_H

#include <stdint.h>
#include <assert.h>

#include <vector>
#include <string>
#include <utility>

#include <functional>

#include <cel/celfile.h>
#include <cel/celframe.h>

#include "misc.h"

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

    // Tile mesasured in indexes on tile grid
    struct Tile
    {
      int32_t x;
      int32_t y;
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

    void init(const RenderSettings& settings, NuklearGraphicsContext& nuklearGraphics, nk_context* nk_ctx);

    void destroyNuklearGraphicsContext(NuklearGraphicsContext& nuklearGraphics);
    void quit(); 

    void resize(size_t w, size_t h);
    RenderSettings getWindowSize();
    void drawGui(NuklearFrameDump& dump, SpriteCacheBase* cache);

    bool getImageInfo(const std::string& path, uint32_t& width, uint32_t& height, uint32_t& animLength, int32_t celIndex=0);
    void drawCursor(Sprite s, size_t w=0, size_t h=0);
    SpriteGroup* loadSprite(const std::string& path, bool hasTrans, size_t transR, size_t transG, size_t transB);
    SpriteGroup* loadVanimSprite(const std::string& path, size_t vAnim, bool hasTrans, size_t transR, size_t transG, size_t transB);
    SpriteGroup* loadResizedSprite(const std::string& path, size_t width, size_t height, size_t tileWidth, size_t tileHeight,  bool hasTrans, size_t transR, size_t transG, size_t transB);
    SpriteGroup* loadCelToSingleTexture(const std::string& path);
    SpriteGroup* loadSprite(const uint8_t* source, size_t width, size_t height);
    SpriteGroup* loadTiledTexture(const std::string& sourcePath, size_t width, size_t height, bool hasTrans, size_t transR, size_t transG, size_t transB);

    void draw();

    void handleEvents();

    void drawSprite(const Sprite& sprite, int32_t x, int32_t y);

    struct RocketFATex
    {
        size_t animLength;
        size_t spriteIndex;
        size_t index;
        bool needsImmortal;
    };

    void spriteSize(const Sprite& sprite, int32_t& w, int32_t& h);

    SpriteGroup* loadTilesetSprite(const std::string& celPath, const std::string& minPath, bool top);
    void drawLevel(const Level::Level& level, size_t minTopsHandle, size_t minBottomsHandle, SpriteCacheBase* cache, LevelObjects& objs, int32_t x1, int32_t y1, int32_t x2, int32_t y2, size_t dist);

    Tile getClickedTile(size_t x, size_t y, int32_t x1, int32_t y1, int32_t x2, int32_t y2, size_t dist);

    void clear(int r = 0, int g = 0, int b = 255);
}

#endif
