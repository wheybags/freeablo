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

#include <misc/disablewarn.h>
    #include <Rocket/Core.h>
#include <misc/enablewarn.h>

#include "rocketglue/drawcommand.h"

#include <SDL.h>

class LevelObjects;
class SpriteGroup;

namespace Render
{
    typedef void* Sprite;
    typedef SDL_Cursor * FACursor;
    typedef SDL_Surface * FASurface;
}

#include "levelobjects.h"

namespace Level
{
    class Level;
}

namespace Render
{
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

    class SpriteCacheBase
    {
        public:
            virtual SpriteGroup* get(uint32_t key) = 0;
            virtual void setImmortal(uint32_t index, bool immortal) = 0;
    };



    void init(const RenderSettings& settings);
    Rocket::Core::Context* initGui(std::function<bool(Rocket::Core::TextureHandle&, Rocket::Core::Vector2i&, const Rocket::Core::String&)> loadTextureFunc,
                                   std::function<bool(Rocket::Core::TextureHandle&, const Rocket::Core::byte*, const Rocket::Core::Vector2i&)> generateTextureFunc,
                                   std::function<void(Rocket::Core::TextureHandle)> releaseTextureFunc);

    void quit();

    void resize(size_t w, size_t h);
    RenderSettings getWindowSize();
    void updateGuiBuffer(std::vector<DrawCommand>* buffer);
    void quitGui();
    void drawGui(std::vector<DrawCommand>& buffer, SpriteCacheBase* cache);

    bool getImageInfo(const std::string& path, uint32_t& width, uint32_t& height, uint32_t& animLength, int32_t celIndex=0);
    void drawCursor(Sprite s, size_t w=0, size_t h=0);
    SpriteGroup* loadSprite(const std::string& path, bool hasTrans, size_t transR, size_t transG, size_t transB);
    SpriteGroup* loadVanimSprite(const std::string& path, size_t vAnim, bool hasTrans, size_t transR, size_t transG, size_t transB);
    SpriteGroup* loadResizedSprite(const std::string& path, size_t width, size_t height, size_t tileWidth, size_t tileHeight,  bool hasTrans, size_t transR, size_t transG, size_t transB);
    SpriteGroup* loadCelToSingleTexture(const std::string& path);
    SpriteGroup* loadSprite(const uint8_t* source, size_t width, size_t height);
    SpriteGroup* loadTiledTexture(const std::string& sourcePath, size_t width, size_t height, bool hasTrans, size_t transR, size_t transG, size_t transB);

    void draw();

    void drawSprite(const Sprite& sprite, size_t x, size_t y, boost::optional<Cel::Colour> color = boost::none);

    class SpriteGroup
    {
        public:
            SpriteGroup(const std::string& path);
            SpriteGroup(const std::vector<Sprite> sprites): mSprites(sprites), mAnimLength(sprites.size()) {}
            void destroy();

            Sprite& operator[](size_t index)
            {
                #ifndef NDEBUG
                    assert(index < mSprites.size());
                #endif
                return mSprites[index];
            }

            size_t size()
            {
                return mSprites.size();
            }

            size_t animLength()
            {
                return mAnimLength;
            }

            static void toPng(const std::string& celPath, const std::string& pngPath);
            const std::vector<Sprite> &masks () { return mMasks; }

        private:
            std::vector<Sprite> mSprites;
            std::vector<Sprite> mMasks; // used for highlighting during hovering
            size_t mAnimLength;
    };

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
