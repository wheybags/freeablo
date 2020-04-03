#pragma once
#include "spritecache.h"
#include <set>

class Image;

namespace FARender
{
    ///
    /// @brief Manager for game sprites. Mostly just a wrapper for SpriteCache, see that for documentation of members.
    ///
    /// Inherits from Render::SpriteCacheBase so it can be passed into Render functions and be used as a cache.
    ///
    class SpriteManager : public Render::SpriteCacheBase
    {
    public:
        explicit SpriteManager(uint32_t cacheSize);

        //////////////////////////////////
        // game thread public functions //
        //////////////////////////////////

        /// Like get(const std::string&), but for use directly with a pixel buffer
        /// @brief To be called from the game thread
        FASpriteGroup* getFromRaw(Image&& image);

        bool getAndClearSpritesNeedingPreloading(std::vector<uint32_t>& sprites); ///< To be called from the game thread

        /////////////////////////////
        // render thread functions //
        /////////////////////////////

        /// Wrapper for SpriteCache::get(uint32_t, bool)
        /// When loading a normal sprite reference, will just pass through, otherwise the reference is a raw load reference from getFromRaw above,
        /// so will load that and inject it into mCache with SpriteCache::directInsert
        /// @brief To be called from the render thread
        Render::SpriteGroup* get(uint32_t index) override;

        void setImmortal(uint32_t index, bool immortal) override; ///< To be called from the render thread

        void clear(); ///< To be called from the render thread

    private:
        SpriteCache mCache;

        void addToPreloadList(uint32_t index); ///< To be called from the game thread

        std::map<uint32_t, Image> mRawCache;
        std::vector<FASpriteGroup*> mRawSpriteGroups;
        std::vector<uint32_t> mSpritesNeedingPreloading;
        std::set<uint32_t> mSpritesAlredyPreloaded;
    };
}
