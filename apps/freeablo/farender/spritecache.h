#ifndef SPRITE_CACHE_H
#define SPRITE_CACHE_H

#include <stdlib.h>
#include <map>
#include <vector>
#include <string>
#include <list>
#include <utility>

#include <render/render.h>

namespace FARender
{
    class Renderer;
    class SpriteCache;
    class FASpriteGroup
    {
        public:
            size_t animLength;
            FASpriteGroup() {} ///< Do not call, just added to satisfy std::map
        private:
            size_t spriteCacheIndex;
            FASpriteGroup(size_t _len, size_t _index): animLength(_len), spriteCacheIndex(_index) {}
            friend class Renderer;
            friend class SpriteCache;
    };


    class SpriteCache : public Render::SpriteCacheBase
    {
        public:
            SpriteCache(size_t size);

            FASpriteGroup get(const std::string& path); ///< To be called from the game thread
            Render::SpriteGroup* get(size_t index); ///< To be called from the render thread

            void clear(); //< To be called from the render thread

        private:
            void evict();

            std::map<std::string, FASpriteGroup> mStrToCache;
            std::map<size_t, std::string> mCacheToStr;

            std::map<size_t, size_t> mCacheToSprite;

            std::map<size_t, std::pair<Render::SpriteGroup*, std::list<size_t>::iterator> > mCache;
            std::list<size_t> mUsedList;

            size_t mNextCacheIndex;

            size_t mCurrentSize;
            size_t mMaxSize;
    };
}

#endif
