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
            FASpriteGroup(): spriteCacheIndex(0) {}
        private:
            size_t spriteCacheIndex;
            FASpriteGroup(size_t _len, size_t _index): animLength(_len), spriteCacheIndex(_index) {}
            friend class Renderer;
            friend class SpriteCache;
    };

    struct TilesetPath
    {
        std::string celPath;
        std::string minPath;
        bool top;

        TilesetPath(std::string _c, std::string _m, bool _t)
            :celPath(_c), minPath(_m), top(_t) {}

        TilesetPath() {}
    };

    struct CacheEntry
    {
        Render::SpriteGroup* sprite;
        std::list<size_t>::iterator it;
        bool immortal;

        CacheEntry(Render::SpriteGroup* _sprite, std::list<size_t>::iterator _it, bool _immortal)
            :sprite(_sprite), it(_it), immortal(_immortal) {}

        CacheEntry() {}
    };

    class SpriteCache : public Render::SpriteCacheBase
    {
        public:
            SpriteCache(size_t size);

            FASpriteGroup get(const std::string& path); ///< To be called from the game thread
            FASpriteGroup getTileset(const std::string& celPath, const std::string& minPath, bool top); ///< Same as above, but for tileset sprites
                                                                                                        ///< To be called from the game thread

            Render::SpriteGroup* get(size_t index); ///< To be called from the render thread

            ///< To be called from the render thread
            ///< Used when we need to guarantee that a sprite will not be evicted for a period of time
            void setImmortal(size_t index, bool immortal);

            void clear(); //< To be called from the render thread

        private:
            void moveToFront(size_t index);
            void evict();

            std::map<std::string, FASpriteGroup> mStrToCache;
            std::map<size_t, std::string> mCacheToStr;

            std::map<std::string, FASpriteGroup> mStrToTilesetCache;
            std::map<size_t, TilesetPath> mCacheToTilesetPath;

            std::map<size_t, size_t> mCacheToSprite;

            std::map<size_t, CacheEntry> mCache;
            std::list<size_t> mUsedList;

            size_t mNextCacheIndex;

            size_t mCurrentSize;
            size_t mMaxSize;
    };
}

#endif
