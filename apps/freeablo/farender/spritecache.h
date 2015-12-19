#ifndef SPRITE_CACHE_H
#define SPRITE_CACHE_H

#include <stdlib.h>
#include <map>
#include <vector>
#include <string>
#include <list>
#include <utility>
#include <atomic>

#include <render/render.h>

namespace FARender
{
    class Renderer;
    class SpriteCache;
    class FASpriteGroup
    {
        public:
            size_t animLength;
            size_t width;
            size_t height;
            FASpriteGroup(): spriteCacheIndex(0) {}
        
            bool isValid()
            {
                return spriteCacheIndex != 0;
            }
        private:
            size_t spriteCacheIndex;
            friend class Renderer;
            friend class SpriteCache;
            friend class SpriteManager;
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

    ///
    /// @brief Multithread sprite cache
    ///
    /// FASpriteGroup is returned from the get() functions used in the game thread, which do not actually do image loading.
    /// get(size_t index) method is used to get a Render::SpriteGroup pointer in the render thread, actual image loading is done lazily here.
    /// The index value comes from FASpriteGroup.spriteCacheIndex
    ///
    class SpriteCache
    {
        public:
            SpriteCache(size_t size);

            FASpriteGroup get(const std::string& path); ///< To be called from the game thread

            /// Same as get(const std::string&), but for tileset sprites
            /// @brief To be called from the game thread
            FASpriteGroup getTileset(const std::string& celPath, const std::string& minPath, bool top);

            size_t newUniqueIndex(); ///< Can be called from any thread

            /// Directly inserts a sprite into the cache at the specified index.
            /// Does not touch any of the m*toCache members, so the only way to get a reference is to have one from before
            /// @brief To be called from the render thread
            void directInsert(Render::SpriteGroup* sprite, size_t cacheIndex);

            /// Gets a Render::SpriteGroup* from the cache if it has been loaded before, otherwise load it,
            /// insert it into the cache, and then return it.
            ///
            /// @brief To be called from the render thread
            Render::SpriteGroup* get(size_t index);

            /// Used when we need to guarantee that a sprite will not be evicted for a period of time
            /// @brief To be called from the render thread
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

            std::atomic<size_t> mNextCacheIndex;

            size_t mCurrentSize;
            size_t mMaxSize;
    };
}

#endif
