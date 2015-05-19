#ifndef RENDER_H
#define RENDER_H

#include <stdint.h>

#include <vector>
#include <string>
#include <utility>

#include <boost/function.hpp>

#include <cel/celfile.h>
#include <cel/celframe.h>

#include <misc/disablewarn.h>
    #include <Rocket/Core.h>
#include <misc/enablewarn.h>

#include "rocketglue/drawcommand.h"

namespace Render
{
    typedef void* Sprite;
}

#include "levelobjects.h"

namespace Level
{
    class Level;
}

namespace Render
{
    /**
     * @brief Render settings for initialization.
     */
    struct RenderSettings
    {
        int32_t windowWidth;
        int32_t windowHeight;
    };

    class SpriteCacheBase
    {
        public:
            virtual SpriteGroup* get(size_t key) = 0;
            virtual void setImmortal(size_t index, bool immortal) = 0;
    };



    void init(const RenderSettings& settings); 
    Rocket::Core::Context* initGui(boost::function<bool(Rocket::Core::TextureHandle&, Rocket::Core::Vector2i&, const Rocket::Core::String&)> loadTextureFunc,
                                   boost::function<bool(Rocket::Core::TextureHandle&, const Rocket::Core::byte*, const Rocket::Core::Vector2i&)> generateTextureFunc,
                                   boost::function<void(Rocket::Core::TextureHandle)> releaseTextureFunc);

    void quit(); 

	void resize(size_t w, size_t h);
   
    void updateGuiBuffer(std::vector<DrawCommand>* buffer);
    void quitGui();
    void drawGui(std::vector<DrawCommand>& buffer, SpriteCacheBase* cache);

    bool getImageInfo(const std::string& path, size_t& width, size_t& height, size_t& animLength, int32_t celIndex=0);
    SpriteGroup* loadSprite(const std::string& path, bool hasTrans, size_t transR, size_t transG, size_t transB);
    SpriteGroup* loadVanimSprite(const std::string& path, size_t vAnim, bool hasTrans, size_t transR, size_t transG, size_t transB);
    SpriteGroup* loadSprite(const uint8_t* source, size_t width, size_t height);
    Sprite loadVideoFrame(uint8_t* data[], int * linesize, size_t width, size_t height);

    void draw();

    void drawAt(const Sprite& sprite, size_t x, size_t y);

    class SpriteGroup
    {
        public:
            SpriteGroup(const std::string& path);
            SpriteGroup(const std::vector<Sprite> sprites): mSprites(sprites), mAnimLength(sprites.size()) {}
            void destroy();
            
            Sprite& operator[](size_t index)
            {
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

        private:
            std::vector<Sprite> mSprites;
            size_t mAnimLength;
    };

    struct RocketFATex
    {
        size_t animLength;
        size_t spriteIndex;
        size_t index;
        bool needsImmortal;
    };

    void spriteSize(const Sprite& sprite, size_t& w, size_t& h);

    SpriteGroup* loadTilesetSprite(const std::string& celPath, const std::string& minPath, bool top);
    void drawLevel(const Level::Level& level, size_t minTopsHandle, size_t minBottomsHandle, SpriteCacheBase* cache, LevelObjects& objs, int32_t x1, int32_t y1, int32_t x2, int32_t y2, size_t dist);
    
    std::pair<size_t, size_t> getClickedTile(const Level::Level& level, size_t x, size_t y, int32_t x1, int32_t y1, int32_t x2, int32_t y2, size_t dist);

    void clear();
}

#endif
