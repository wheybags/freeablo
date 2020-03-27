#include "spritemanager.h"
#include <Image/image.h>
#include <cstring>

namespace FARender
{
    SpriteManager::SpriteManager(uint32_t cacheSize) : mCache(cacheSize) {}

    ///////////////////////////
    // game thread functions //
    ///////////////////////////

    FASpriteGroup* SpriteManager::get(const std::string& path, bool trim)
    {
        auto tmp = mCache.get(path, trim);
        addToPreloadList(tmp->spriteCacheIndex);
        return tmp;
    }

    FASpriteGroup* SpriteManager::getTileset(const std::string& celPath, const std::string& minPath, bool top, bool trim)
    {
        auto tmp = mCache.getTileset(celPath, minPath, top, trim);
        addToPreloadList(tmp->spriteCacheIndex);
        return tmp;
    }

    std::string SpriteManager::getPathForIndex(uint32_t index) { return mCache.getPathForIndex(index); }

    FASpriteGroup* SpriteManager::getFromRaw(Image&& image)
    {
        release_assert(image.mData.ownsData());

        uint32_t index = mCache.newUniqueIndex();

        FASpriteGroup* retval = mCache.allocNewSpriteGroup();
        retval->init(1, {image.width()}, {image.height()}, index);

        mRawCache[index] = std::move(image);

        // put it in a member vector because we need to return a persistent pointer
        mRawSpriteGroups.push_back(retval);

        addToPreloadList(retval->spriteCacheIndex);

        return retval;
    }

    bool SpriteManager::getAndClearSpritesNeedingPreloading(std::vector<uint32_t>& sprites)
    {
        sprites = mSpritesNeedingPreloading;
        mSpritesNeedingPreloading.clear();
        return sprites.size() != 0;
    }

    void SpriteManager::addToPreloadList(uint32_t index)
    {
        if (mSpritesAlredyPreloaded.count(index) == 0)
        {
            mSpritesAlredyPreloaded.insert(index);
            mSpritesNeedingPreloading.push_back(index);
        }
    }

    /////////////////////////////
    // render thread functions //
    /////////////////////////////

    Render::SpriteGroup* SpriteManager::get(uint32_t index)
    {
        if (mRawCache.count(index))
        {
            Image image = std::move(mRawCache[index]);
            mRawCache.erase(index);

            Render::SpriteGroup* newSprite = Render::loadSprite(image, false);
            mCache.directInsert(newSprite, index);

            return newSprite;
        }

        return mCache.get(index);
    }

    void SpriteManager::setImmortal(uint32_t index, bool immortal)
    {
        get(index);
        mCache.setImmortal(index, immortal);
    }

    void SpriteManager::clear() { mCache.clear(); }
}
