#include "spritemanager.h"

#include <cstring>

namespace FARender
{
    SpriteManager::SpriteManager(uint32_t cacheSize): mCache(cacheSize) {}

    ///////////////////////////
    // game thread functions //
    ///////////////////////////

    FASpriteGroup* SpriteManager::get(const std::string& path)
    {
        auto tmp = mCache.get(path);
        addToPreloadList(tmp->spriteCacheIndex);
        return tmp;
    }

    FASpriteGroup* SpriteManager::getTileset(const std::string& celPath, const std::string& minPath, bool top)
    {
        auto tmp = mCache.getTileset(celPath, minPath, top);
        addToPreloadList(tmp->spriteCacheIndex);
        return tmp;
    }

    FASpriteGroup* SpriteManager::getByServerSpriteIndex(uint32_t index)
    {
        if(!mServerSpriteMap.count(index))
        {
            FASpriteGroup* newSprite = mCache.allocNewSpriteGroup();
            mServerSpriteMap[index] = newSprite;
        }

        return mServerSpriteMap[index];
    }

    std::string SpriteManager::getPathForIndex(uint32_t index)
    {
        return mCache.getPathForIndex(index);
    }

    void SpriteManager::fillServerSprite(uint32_t serverIndex, const std::string& path)
    {
        auto source = get(path);
        auto dest = getByServerSpriteIndex(serverIndex);

        *dest = *source;
    }

    FASpriteGroup* SpriteManager::getFromRaw(const uint8_t* source, uint32_t width, uint32_t height)
    {
        uint32_t size = (width*4)*height;

        uint8_t* buffer = new uint8_t[size];
        memcpy(buffer, source, size);

        uint32_t index = mCache.newUniqueIndex();
        RawCacheTmp rawTmp;
        rawTmp.buffer = buffer;
        rawTmp.width = width;
        rawTmp.height = height;

        mRawCache[index] = rawTmp;

        FASpriteGroup* retval = mCache.allocNewSpriteGroup();
        retval->init(1, {static_cast<int32_t> (width)}, {static_cast<int32_t> (height)}, index);

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
        if(mRawCache.count(index))
        {
            RawCacheTmp tmp = mRawCache[index];

            Render::SpriteGroup* newSprite = Render::loadSprite(tmp.buffer, tmp.width, tmp.height);
            delete[] tmp.buffer;

            mCache.directInsert(newSprite, index);
            mRawCache.erase(index);

            return newSprite;
        }

        return mCache.get(index);
    }

    void SpriteManager::setImmortal(uint32_t index, bool immortal)
    {
        get(index);
        mCache.setImmortal(index, immortal);
    }

    void SpriteManager::clear()
    {
        mCache.clear();
    }
}
