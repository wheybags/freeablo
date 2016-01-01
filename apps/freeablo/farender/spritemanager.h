#ifndef SPRITE_MANAGER_H
#define SPRITE_MANAGER_H

#include "spritecache.h"

namespace FARender
{
    struct RawCacheTmp
    {
        uint8_t* buffer;
        size_t width;
        size_t height;
    };

    ///
    /// @brief Manager for game sprites. Mostly just a wrapper for SpriteCache, see that for documentation of members.
    ///
    /// Inherits from Render::SpriteCacheBase so it can be passed into Render functions and be used as a cache.
    ///
    class SpriteManager : public Render::SpriteCacheBase
    {
        public:
            SpriteManager(size_t cacheSize);

            FASpriteGroup* get(const std::string& path); ///< To be called from the game thread
            FASpriteGroup* getTileset(const std::string& celPath, const std::string& minPath, bool top); ///< To be called from the game thread

            /// Will return a blank FASpriteGroup on first calling, that can be filled in with it's real source later,
            /// with fillServerSprite. After it has been filled, all subsequent calls will return a valid FASpriteGroup
            /// for the source it has been filled with.
            /// @brief To be called from the game thread
            FASpriteGroup* getByServerSpriteIndex(size_t index);

            /// Used by NetManager on the server to get the paths to send to clients
            /// for them to user with fillServerSprite
            /// @brief To be called from the game thread
            std::string getPathForIndex(size_t index);

            /// See getByServerSpriteIndex above
            /// @brief To be called from the game thread
            void fillServerSprite(size_t serverIndex, const std::string& path);

            /// Like get(const std::string&), but for use directly with a pixel buffer
            /// @brief To be called from the game thread
            FASpriteGroup* getFromRaw(const uint8_t* source, size_t width, size_t height);

            /// Wrapper for SpriteCache::get(size_t, bool)
            /// When loading a normal sprite reference, will just pass through, otherwise the reference is a raw load reference from getFromRaw above,
            /// so will load that and inject it into mCache with SpriteCache::directInsert
            /// @brief To be called from the render thread
            Render::SpriteGroup* get(size_t index);

            void setImmortal(size_t index, bool immortal); ///< To be called from the render thread

            void clear(); ///< To be called from the render thread

       private:
            SpriteCache mCache;

            std::map<size_t, RawCacheTmp> mRawCache;
            std::vector<FASpriteGroup*> mRawSpriteGroups;
            std::map<size_t, FASpriteGroup*> mServerSpriteMap;
    };
}

#endif
