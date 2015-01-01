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

#include "rocketglue/RenderInterfaceSDL2.h"

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

    void init(const RenderSettings& settings); 
    Rocket::Core::Context* initGui(boost::function<bool(Rocket::Core::TextureHandle&, Rocket::Core::Vector2i&, const Rocket::Core::String&)> loadTextureFunc,
                                   boost::function<bool(Rocket::Core::TextureHandle&, const Rocket::Core::byte*, const Rocket::Core::Vector2i&)> generateTextureFunc,
                                   boost::function<void(Rocket::Core::TextureHandle)> releaseTextureFunc);

    void quit(); 

	void resize(size_t w, size_t h);
   
    void updateGuiBuffer(std::vector<drawCommand>& buffer);
    void drawGui(std::vector<drawCommand>& buffer); 
    
    bool guiLoadImage(Rocket::Core::TextureHandle& texture_handle, Rocket::Core::Vector2i& texture_dimensions, const Rocket::Core::String& source);
	bool guiGenerateTexture(Rocket::Core::TextureHandle& texture_handle, const Rocket::Core::byte* source, const Rocket::Core::Vector2i& source_dimensions);
    void guiReleaseTexture(Rocket::Core::TextureHandle texture_handle);

    void draw();

    void drawAt(const Sprite& sprite, size_t x, size_t y); 

    class SpriteGroup
    {
        public:
            SpriteGroup(const std::string& path);
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

    class SpriteCacheBase
    {
        public:
            virtual SpriteGroup* get(size_t key) = 0;
    };

    void spriteSize(const Sprite& sprite, size_t& w, size_t& h);



    struct RenderLevel
    {
        public:
            ~RenderLevel();

        private:
            const Level::Level* level;
            std::map<int32_t, Sprite> minTops;
            std::map<int32_t, Sprite> minBottoms;
            int32_t levelHeight;
            int32_t levelWidth;
            int32_t levelX;
            int32_t levelY;

            friend RenderLevel* setLevel(const Level::Level& level);
            friend void drawLevel(RenderLevel* level, SpriteCacheBase* cache, LevelObjects& objs, int32_t x1, int32_t y1, int32_t x2, int32_t y2, size_t dist);
            friend void drawLevelHelper(RenderLevel* level, std::map<int32_t, Sprite>& minMap, int32_t x, int32_t y);
            friend void drawAt(RenderLevel* level, const Sprite& sprite, int32_t x1, int32_t y1, int32_t x2, int32_t y2, size_t dist);
            friend std::pair<size_t, size_t> getClickedTile(RenderLevel* level, size_t x, size_t y);
    };

    RenderLevel* setLevel(const Level::Level& level);
    void drawLevel(RenderLevel* level, SpriteCacheBase* cache, LevelObjects& objs, int32_t x1, int32_t y1, int32_t x2, int32_t y2, size_t dist);
    void drawAt(RenderLevel* level, const Sprite& sprite, int32_t x1, int32_t y1, int32_t x2, int32_t y2, size_t dist);
    
    std::pair<size_t, size_t> getClickedTile(RenderLevel* level, size_t x, size_t y);

    void clear();
}

#endif
